#pragma once

#include <format>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

#include <axgl/common.hpp>
#include <axgl/interface/renderer.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/context_holder.hpp>
#include <axgl/impl/glfw/window.hpp>
#include <axgl/impl/opengl/renderer/blend.hpp>
#include <axgl/impl/opengl/renderer/bloom.hpp>
#include <axgl/impl/opengl/renderer/hdr.hpp>
#include <axgl/impl/opengl/renderer/msaa.hpp>
#include <axgl/impl/opengl/renderer/render_component.hpp>
#include <axgl/impl/opengl/renderer/screen.hpp>
#include <axgl/impl/opengl/renderer/shadow.hpp>
#include <axgl/impl/opengl/renderer/ssao.hpp>
#include <axgl/impl/opengl/shaders.hpp>
#include <axgl/impl/opengl/texture.hpp>

#include <opengl/framebuffer.hpp>
#include <opengl/renderbuffer.hpp>
#include <opengl/static_vaos.hpp>

namespace axgl::impl::opengl
{

class Renderer : virtual public axgl::Renderer, public axgl::impl::ContextHolder
{
  using RenderComponents
    = std::unordered_map<std::uint64_t, renderer::RenderComponent*>;

  bool initialized_glad_ = false;
  axgl::ptr_t<glfw::Window> window_;

  glm::vec2 viewport_{0.0f};

  renderer::HDR hdr;
  renderer::MSAA msaa;
  renderer::SSAO ssao;
  renderer::Blend blend;
  renderer::Bloom bloom;
  renderer::Shadow shadow;
  renderer::Screen screen;

public:
  void render() override
  {
    if (!window_ || !window_->ready())
    {
      AXGL_LOG_DEBUG("Unable to render: window is not set or not ready.");
      return;
    }

    const auto& gui = axgl_->gui_service()->get_main_ui();
    const auto& realm = axgl_->realm_service()->get_active_realm();

    const auto viewport_i = window_->get_size();
    const auto viewport_f = glm::vec2(viewport_i);
    // setup
    if (viewport_ != viewport_f)
    {
      viewport_ = viewport_f;
      screen.setup(viewport_i);
      if (msaa.enabled) msaa.setup(viewport_i);
      if (blend.enabled) blend.setup(viewport_i, *screen.depth_texture);
      if (hdr.enabled) hdr.setup(viewport_i);
      if (ssao.enabled) ssao.setup(viewport_i, *screen.depth_texture);
      if (bloom.enabled) bloom.setup(viewport_i);
      if (gui)
      {
        gui->set_size(viewport_i.x, viewport_i.y);
        gui->init();
      }
    }
    msaa.update(viewport_i);
    blend.update(viewport_i, *screen.depth_texture);
    hdr.update(viewport_i);
    shadow.update();
    ssao.update(viewport_i, *screen.depth_texture);
    bloom.update(viewport_i);

    // render realm
    auto* camera = axgl_->camera_service()->get_camera();
    if (camera && camera->viewport != viewport_f)
    {
      camera->viewport.x = viewport_f.x;
      camera->viewport.y = viewport_f.y;
      camera->update_projection_view_matrix();
    }
    if (camera && realm)
    {
      renderer::RenderContext render_context{
        .viewport = viewport_f,
        .viewpoint = camera->position,
        .view_matrix = camera->view_matrix(),
        .projection_matrix = camera->projection_matrix(),
        .projection_view_matrix = camera->projection_view_matrix(),
        .inverse_projection_view_matrix
        = glm::inverse(camera->projection_view_matrix()),
        .camera_near = camera->near_clip,
        .camera_far = camera->far_clip,
#ifdef AXGL_DEBUG
        .csm_debug_borders = shadow.enable_csm_debug,
#endif
      };

      RenderComponents render_components;
      {
        AXGL_PROFILE_SCOPE("Renderer Gather Instances");
        gather_render_components(
          render_context, render_components, realm->entities()
        );
        AXGL_PLOT("Renderer Entity Count", render_context.entity_count);
        AXGL_PLOT("Renderer Component Count", render_context.component_count);
        AXGL_PLOT(
          "Renderer Light Count",
          static_cast<std::int64_t>(render_context.sun_lights.size())
        );
      }
      renderer::PipelineContext pipeline_context;
      {
        AXGL_PROFILE_SCOPE("Renderer Submit Calls");
        for (auto* render_comp : render_components | std::views::values)
          render_comp->submit_render_function(pipeline_context);
      }

      shadow.render_shadow_pass(render_context, pipeline_context);
      render_ssao_pass(render_context, pipeline_context, viewport_i);
      render_opaque_pass(render_context, pipeline_context, viewport_i);
      render_transparent_pass(render_context, pipeline_context);
    }
    else
    {
      // only clear screen texture if no camera or realm exists
      screen.screen_framebuffer->use();
      glViewport(0, 0, viewport_i.x, viewport_i.y);
      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT);
    }

    // render screen
    if (bloom.enabled) render_bloom_pass(viewport_i);
    if (hdr.enabled) render_tone_mapping_pass(viewport_i);
    render_gui(gui);
    render_to_screen();
    window_->swap_buffers();
  }

private:
  void render_opaque_pass(
    const renderer::RenderContext& render_context,
    const renderer::PipelineContext& pipeline_context,
    const glm::ivec2& viewport_i
  )
  {
    glViewport(0, 0, viewport_i.x, viewport_i.y);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glDepthRange(0.0f, 1.0f);

    if (msaa.enabled) msaa.msaa_framebuffer->use();
    else screen.screen_framebuffer->use();
    glClearDepth(1.0);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    {
      AXGL_PROFILE_SCOPE("Renderer Opaque Pass");
      for (const auto& render_func : pipeline_context.opaque_pass)
        render_func(render_context);
    }

    if (msaa.enabled)
    {
      AXGL_PROFILE_SCOPE("Renderer MSAA Resolve");
      msaa.msaa_framebuffer->use_read();
      screen.screen_framebuffer->use_write();
      glBlitFramebuffer(
        0, 0, viewport_i.x, viewport_i.y,                     //
        0, 0, viewport_i.x, viewport_i.y,                     //
        GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST //
      );
    }
  }

  void render_ssao_pass(
    renderer::RenderContext& render_context,
    const renderer::PipelineContext& pipeline_context,
    const glm::ivec2& viewport_i
  )
  {
    if (!ssao.enabled || pipeline_context.geometry_pass.empty()) return;

    // geometry pass: render view-space position + normal to the SSAO g-buffer
    {
      AXGL_PROFILE_SCOPE("Renderer SSAO Geometry Pass");
      ssao.geometry_framebuffer->use();
      glViewport(0, 0, viewport_i.x, viewport_i.y);
      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LESS);
      glDepthMask(GL_TRUE);
      glDisable(GL_BLEND);
      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      for (const auto& render_func : pipeline_context.geometry_pass)
        render_func(render_context);
    }

    // SSAO generation pass: sample hemisphere against the g-buffer
    {
      AXGL_PROFILE_SCOPE("Renderer SSAO Generation Pass");
      ssao.ssao_framebuffer->use();
      glViewport(0, 0, viewport_i.x, viewport_i.y);
      glDisable(GL_DEPTH_TEST);
      glDisable(GL_BLEND);
      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT);
      ssao.position_texture->use(GL_TEXTURE0);
      ssao.normal_texture->use(GL_TEXTURE1);
      ssao.noise_texture->use(GL_TEXTURE2);
      const auto& shader = Shaders::instance().ssao();
      shader.use_program();
      shader.set_int("position_texture", 0);
      shader.set_int("normal_texture", 1);
      shader.set_int("noise_texture", 2);
      shader.set_mat4("projection_matrix", render_context.projection_matrix);
      shader.set_vec2("noise_scale", ssao.noise_scale(viewport_i));
      shader.set_float("radius", ssao.radius);
      shader.set_float("bias", ssao.bias);
      for (std::size_t i = 0; i < renderer::kSsaoKernelSize; ++i)
        shader.set_vec3(std::format("kernel_samples[{}]", i), ssao.kernel[i]);
      ::opengl::StaticVAOs::instance().quad().draw();
    }

    // blur pass: 5x5 box filter to remove the noise tiling pattern
    {
      AXGL_PROFILE_SCOPE("Renderer SSAO Blur Pass");
      ssao.blur_framebuffer->use();
      glViewport(0, 0, viewport_i.x, viewport_i.y);
      glDisable(GL_DEPTH_TEST);
      glDisable(GL_BLEND);
      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT);
      ssao.ssao_texture->use(GL_TEXTURE0);
      const auto& shader = Shaders::instance().ssao_blur();
      shader.use_program();
      shader.set_int("ssao_texture", 0);
      ::opengl::StaticVAOs::instance().quad().draw();
    }

    render_context.ssao_texture = ssao.blur_texture.get();
  }

  void render_transparent_pass(
    const renderer::RenderContext& render_context,
    const renderer::PipelineContext& pipeline_context
  )
  {
    if (!blend.enabled || pipeline_context.blend_pass.empty()) return;

    glDisable(GL_BLEND);
    glDepthMask(GL_FALSE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    blend.blend_framebuffer->use();
    glClearBufferfv(GL_COLOR, 0, &renderer::Blend::zero_filler[0]);
    glClearBufferfv(GL_COLOR, 1, &renderer::Blend::one_filler[0]);

    glEnable(GL_BLEND);
    glBlendFunci(0, GL_ONE, GL_ONE);
    glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
    glBlendEquation(GL_FUNC_ADD);

    {
      AXGL_PROFILE_SCOPE("Renderer Transparent Pass");
      for (const auto& render_func : pipeline_context.blend_pass)
        render_func(render_context);
    }

    screen.screen_framebuffer->use();
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    blend.accum_texture->use(GL_TEXTURE0);
    blend.reveal_texture->use(GL_TEXTURE1);
    const auto& blend_shader = Shaders::instance().blend();
    blend_shader.use_program();
    blend_shader.set_int("accumulation_texture", 0);
    blend_shader.set_int("reveal_texture", 1);
    ::opengl::StaticVAOs::instance().quad().draw();
  }

  void render_bloom_pass(const glm::ivec2& viewport_i)
  {
    // bright-pass: extract pixels above the luminance threshold from the
    // rendered scene (screen_texture, pre-tone-map HDR)
    {
      AXGL_PROFILE_SCOPE("Renderer Bloom Bright Pass");
      bloom.bright_framebuffer->use();
      glViewport(0, 0, viewport_i.x, viewport_i.y);
      glDisable(GL_BLEND);
      glDisable(GL_DEPTH_TEST);
      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT);
      screen.screen_texture->use(GL_TEXTURE0);
      const auto& shader = Shaders::instance().bloom_bright_pass();
      shader.use_program();
      shader.set_int("screen_texture", 0);
      shader.set_float("threshold", bloom.threshold);
      ::opengl::StaticVAOs::instance().quad().draw();
    }

    // downsample + blur: for each mip level, downsample the previous result
    // then do 2 iterations of separable Gaussian blur (horizontal + vertical)
    // via ping-pong between the two textures at that level.
    {
      AXGL_PROFILE_SCOPE("Renderer Bloom Blur");

      // downsample bright_texture into mip level 0
      const auto& downsample_shader = Shaders::instance().bloom_blur();
      downsample_shader.use_program();

      // first mip: downsample from bright_texture
      {
        const auto mip_dim = bloom.mip_size(0);
        bloom.mip_framebuffers[0][0]->use();
        glViewport(0, 0, mip_dim.x, mip_dim.y);
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        bloom.bright_texture->use(GL_TEXTURE0);
        downsample_shader.use_program();
        downsample_shader.set_int("source_texture", 0);
        downsample_shader.set_vec2("texel_size", 1.0f / glm::vec2(mip_dim));
        downsample_shader.set_vec2("direction", glm::vec2(0.0f));
        ::opengl::StaticVAOs::instance().quad().draw();
      }

      // downsample further mips
      for (std::size_t i = 1; i < renderer::kBloomMipLevels; ++i)
      {
        const auto mip_dim = bloom.mip_size(i);
        bloom.mip_framebuffers[i][0]->use();
        glViewport(0, 0, mip_dim.x, mip_dim.y);
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        bloom.mip_textures[i - 1][0]->use(GL_TEXTURE0);
        downsample_shader.use_program();
        downsample_shader.set_int("source_texture", 0);
        downsample_shader.set_vec2("texel_size", 1.0f / glm::vec2(mip_dim));
        downsample_shader.set_vec2("direction", glm::vec2(0.0f));
        ::opengl::StaticVAOs::instance().quad().draw();
      }

      // blur each mip level with ping-pong (3 iterations per axis)
      for (std::size_t i = 0; i < renderer::kBloomMipLevels; ++i)
      {
        const auto mip_dim = bloom.mip_size(i);
        const glm::vec2 texel_size = 1.0f / glm::vec2(mip_dim);

        for (int iter = 0; iter < 3; ++iter)
        {
          // horizontal blur: [i][0] -> [i][1]
          bloom.mip_framebuffers[i][1]->use();
          glViewport(0, 0, mip_dim.x, mip_dim.y);
          glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
          glClear(GL_COLOR_BUFFER_BIT);
          bloom.mip_textures[i][0]->use(GL_TEXTURE0);
          downsample_shader.use_program();
          downsample_shader.set_int("source_texture", 0);
          downsample_shader.set_vec2("texel_size", texel_size);
          downsample_shader.set_vec2("direction", glm::vec2(1.0f, 0.0f));
          ::opengl::StaticVAOs::instance().quad().draw();

          // vertical blur: [i][1] -> [i][0]
          bloom.mip_framebuffers[i][0]->use();
          glViewport(0, 0, mip_dim.x, mip_dim.y);
          glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
          glClear(GL_COLOR_BUFFER_BIT);
          bloom.mip_textures[i][1]->use(GL_TEXTURE0);
          downsample_shader.use_program();
          downsample_shader.set_int("source_texture", 0);
          downsample_shader.set_vec2("texel_size", texel_size);
          downsample_shader.set_vec2("direction", glm::vec2(0.0f, 1.0f));
          ::opengl::StaticVAOs::instance().quad().draw();
        }
      }
    }

    // composite: add bloom back into a separate texture (can't read and write
    // screen_texture at the same time — that's feedback-undefined), then blit
    // the result back into screen_texture for the tone-map pass.
    {
      AXGL_PROFILE_SCOPE("Renderer Bloom Composite");
      bloom.composite_framebuffer->use();
      glViewport(0, 0, viewport_i.x, viewport_i.y);
      glDisable(GL_BLEND);
      glDisable(GL_DEPTH_TEST);
      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT);
      screen.screen_texture->use(GL_TEXTURE0);
      // bind the smallest mip (most blurred) as the bloom contribution
      bloom.mip_textures[renderer::kBloomMipLevels - 1][0]->use(GL_TEXTURE1);
      const auto& shader = Shaders::instance().bloom_composite();
      shader.use_program();
      shader.set_int("screen_texture", 0);
      shader.set_int("bloom_texture", 1);
      shader.set_float("intensity", bloom.intensity);
      ::opengl::StaticVAOs::instance().quad().draw();

      // copy composite back into screen_texture so tone-mapping sees it
      bloom.composite_framebuffer->use_read();
      screen.screen_framebuffer->use_write();
      glBlitFramebuffer(
        0, 0, viewport_i.x, viewport_i.y, //
        0, 0, viewport_i.x, viewport_i.y, //
        GL_COLOR_BUFFER_BIT, GL_NEAREST   //
      );
    }
  }

  void render_tone_mapping_pass(const glm::ivec2& viewport_i)
  {
    hdr.hdr_framebuffer->use();
    glViewport(0, 0, viewport_i.x, viewport_i.y);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    screen.screen_texture->use(GL_TEXTURE0);
    const auto& screen_shader = Shaders::instance().screen();
    screen_shader.use_program();
    screen_shader.set_int("screen", 0);
    screen_shader.set_bool("enable_hdr", true);
    screen_shader.set_float("exposure", hdr.exposure);
    ::opengl::StaticVAOs::instance().quad().draw();
    screen_shader.set_bool("enable_hdr", false);
  }

  void render_gui(const axgl::ptr_t<axgl::gui::Page>& gui)
  {
    if (!gui) return;

    if (gui->should_render())
    {
      AXGL_PROFILE_SCOPE("GUI Render");
      gui->render();

      const auto& glfw_window = window_->glfw_window();
      switch (gui->get_cursor())
      {
      default:
      case gui::Cursor::kNormal:
        glfw_window->use_standard_cursor(GLFW_ARROW_CURSOR);
        break;
      case gui::Cursor::kText:
        glfw_window->use_standard_cursor(GLFW_IBEAM_CURSOR);
        break;
      case gui::Cursor::kPointer:
        glfw_window->use_standard_cursor(GLFW_POINTING_HAND_CURSOR);
        break;
      case gui::Cursor::kCrosshair:
        glfw_window->use_standard_cursor(GLFW_CROSSHAIR_CURSOR);
        break;
      case gui::Cursor::kResizeVertical:
        glfw_window->use_standard_cursor(GLFW_RESIZE_NS_CURSOR);
        break;
      case gui::Cursor::kResizeHorizontal:
        glfw_window->use_standard_cursor(GLFW_RESIZE_EW_CURSOR);
        break;
      case gui::Cursor::kResizeDiagonalLeft:
        glfw_window->use_standard_cursor(GLFW_RESIZE_NESW_CURSOR);
        break;
      case gui::Cursor::kResizeDiagonalRight:
        glfw_window->use_standard_cursor(GLFW_RESIZE_NWSE_CURSOR);
        break;
      case gui::Cursor::kResize:
        glfw_window->use_standard_cursor(GLFW_CROSSHAIR_CURSOR);
        break;
      case gui::Cursor::kNotAllowed:
        glfw_window->use_standard_cursor(GLFW_NOT_ALLOWED_CURSOR);
        break;
      }
    }

    const auto gui_texture
      = axgl::ptr_cast<axgl::impl::opengl::Texture>(gui->get_texture());
#ifdef AXGL_DEBUG
    if (!gui_texture)
      throw std::runtime_error(
        "axgl::impl::opengl::Texture is required to use "
        "axgl::impl::opengl::Renderer"
      );
#endif
    if (hdr.enabled) hdr.hdr_framebuffer->use();
    else screen.screen_framebuffer->use();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    gui_texture->use(GL_TEXTURE0);
    const auto& screen_shader = Shaders::instance().screen();
    screen_shader.use_program();
    screen_shader.set_int("screen", 0);
    screen_shader.set_bool("enable_hdr", false);
    ::opengl::StaticVAOs::instance().quad().draw();
  }

  void render_to_screen()
  {
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);

    if (!hdr.enabled) glEnable(GL_FRAMEBUFFER_SRGB);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    if (hdr.enabled) hdr.hdr_texture->use(GL_TEXTURE0);
    else screen.screen_texture->use(GL_TEXTURE0);
    const auto& screen_shader = Shaders::instance().screen();
    screen_shader.use_program();
    screen_shader.set_int("screen", 0);
    screen_shader.set_bool("enable_hdr", false);
    ::opengl::StaticVAOs::instance().quad().draw();
    if (!hdr.enabled) glDisable(GL_FRAMEBUFFER_SRGB);
  }

  // TODO: reimplement without recursion
  void gather_render_components(
    renderer::RenderContext& render_context,
    RenderComponents& render_components,
    const axgl::Container<axgl::Entity>& entities,
    const glm::mat4* base_transform_matrix = nullptr
  )
  {
    for (const auto& entity : entities.get())
    {
      if (entity->is_disabled() || entity->is_hidden()) continue;
      ++render_context.entity_count;

      const auto& transform = entity->transform();
      const auto model_matrix = base_transform_matrix
        ? *base_transform_matrix * transform.model_matrix
        : transform.model_matrix;

      for (const auto& component : entity->components().get())
      {
        if (component->is_disabled() || component->is_hidden()) continue;
        ++render_context.component_count;

        if (auto* render_comp
            = dynamic_cast<renderer::RenderComponent*>(component.get()))
        {
          render_comp->gather_instances(model_matrix);

          const auto id = render_comp->get_id();
          render_components[id] = render_comp;
        }
        else if (const auto* light_comp
                 = dynamic_cast<axgl::impl::component::Light*>(component.get()))
        {
          const auto* light = &light_comp->light;
          switch (light->type)
          {
          case axgl::Light::Type::kSun:
            render_context.sun_lights.emplace_back(light);
            break;
          case axgl::Light::Type::kSpot:
            render_context.spot_lights.emplace_back(light);
            break;
          case axgl::Light::Type::kPoint:
            render_context.point_lights.emplace_back(light);
            break;
          }
        }
      }
      if (!entity->children().empty())
      {
        const auto pivot_matrix = base_transform_matrix
          ? *base_transform_matrix * transform.pivot_matrix
          : transform.pivot_matrix;
        gather_render_components(
          render_context, render_components, entity->children(), &pivot_matrix
        );
      }
    }
  }

public:
  //
  // window
  //
  void set_window(axgl::ptr_t<axgl::Window> window) override
  {
    window_ = std::dynamic_pointer_cast<glfw::Window>(std::move(window));
    if (!window_)
#ifdef AXGL_DEBUG
      throw std::runtime_error(
        "The provided window is not a valid GlfwWindow instance. "
        "GlfwWindow is required for OpenglRenderer."
      );
#else
      return;
#endif

    window_->use();

    // initialize glad
    if (!initialized_glad_
        && !gladLoadGLLoader(
          reinterpret_cast<GLADloadproc>(glfwGetProcAddress)
        ))
      AXGL_LOG_ERROR("Failed to initialize GLAD.");
    initialized_glad_ = true;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    window_->swap_buffers();
  }
  [[nodiscard]] axgl::ptr_t<axgl::Window> get_window() const override
  {
    return window_;
  }

  //
  // blend
  //
  void set_enable_blend(bool enable_blend) override
  {
    blend.enabled = enable_blend;
  }
  [[nodiscard]] bool get_enable_blend() const override { return blend.enabled; }

  //
  // msaa
  //
  void set_enable_msaa(bool enable_msaa) override
  {
    msaa.enabled = enable_msaa;
  }
  void set_msaa_sample_count(std::uint32_t msaa_sample_count) override
  {
    GLint max_samples = 0;
    glGetIntegerv(GL_MAX_SAMPLES, &max_samples);
    const auto clamped
      = std::min(msaa_sample_count, static_cast<std::uint32_t>(max_samples));
    if (clamped != msaa_sample_count)
      AXGL_LOG_WARN("GL_MAX_SAMPLES: {}", max_samples);
    msaa.msaa_sample_count = util::narrow<GLsizei>(clamped);
  }
  [[nodiscard]] bool get_enable_msaa() const override { return msaa.enabled; }
  [[nodiscard]] std::uint32_t get_msaa_sample_count() const override
  {
    return util::narrow<std::uint32_t>(msaa.msaa_sample_count);
  }

  //
  // shadow
  //
  void set_enable_shadow(bool enable_shadow) override
  {
    shadow.enabled = enable_shadow;
  }
  void set_shadow_map_size(std::uint32_t shadow_map_size) override
  {
    shadow.shadow_map_size = util::narrow<GLsizei>(shadow_map_size);
  }
  void set_shadow_distance(float shadow_distance) override
  {
    shadow.shadow_distance = shadow_distance;
  }
  [[nodiscard]] bool get_enable_shadow() const override
  {
    return shadow.enabled;
  }
  [[nodiscard]] std::uint32_t get_shadow_map_size() const override
  {
    return util::narrow<std::uint32_t>(shadow.shadow_map_size);
  }
  [[nodiscard]] float get_shadow_distance() const override
  {
    return shadow.shadow_distance;
  }

  // CSM debug
  void set_enable_csm_debug(bool enable_csm_debug)
  {
    shadow.enable_csm_debug = enable_csm_debug;
  }
  [[nodiscard]] bool get_enable_csm_debug() const
  {
    return shadow.enable_csm_debug;
  }

  //
  // HDR
  //
  void set_enable_hdr(bool enable_hdr) override { hdr.enabled = enable_hdr; }
  void set_exposure(float exposure) override { hdr.exposure = exposure; }
  [[nodiscard]] bool get_enable_hdr() const override { return hdr.enabled; }
  [[nodiscard]] float get_exposure() const override { return hdr.exposure; }

  //
  // SSAO
  //
  void set_enable_ssao(bool enable_ssao) override
  {
    ssao.enabled = enable_ssao;
  }
  void set_ssao_radius(float ssao_radius) override
  {
    ssao.radius = ssao_radius;
  }
  void set_ssao_bias(float ssao_bias) override { ssao.bias = ssao_bias; }
  [[nodiscard]] bool get_enable_ssao() const override { return ssao.enabled; }
  [[nodiscard]] float get_ssao_radius() const override { return ssao.radius; }
  [[nodiscard]] float get_ssao_bias() const override { return ssao.bias; }

  //
  // Bloom
  //
  void set_enable_bloom(bool enable_bloom) override
  {
    bloom.enabled = enable_bloom;
  }
  void set_bloom_threshold(float bloom_threshold) override
  {
    bloom.threshold = bloom_threshold;
  }
  void set_bloom_intensity(float bloom_intensity) override
  {
    bloom.intensity = bloom_intensity;
  }
  [[nodiscard]] bool get_enable_bloom() const override { return bloom.enabled; }
  [[nodiscard]] float get_bloom_threshold() const override
  {
    return bloom.threshold;
  }
  [[nodiscard]] float get_bloom_intensity() const override
  {
    return bloom.intensity;
  }
};

} // namespace axgl::impl::opengl
