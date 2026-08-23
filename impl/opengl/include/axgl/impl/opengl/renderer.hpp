#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

#include <axgl/common.hpp>
#include <axgl/interface/renderer.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/context_holder.hpp>
#include <axgl/impl/glfw/window.hpp>
#include <axgl/impl/opengl/renderer/blend.hpp>
#include <axgl/impl/opengl/renderer/hdr.hpp>
#include <axgl/impl/opengl/renderer/msaa.hpp>
#include <axgl/impl/opengl/renderer/render_component.hpp>
#include <axgl/impl/opengl/renderer/shadow_map.hpp>
#include <axgl/impl/opengl/texture.hpp>

#include <opengl/framebuffer.hpp>
#include <opengl/renderbuffer.hpp>
#include <opengl/static_shaders.hpp>
#include <opengl/static_vaos.hpp>

namespace axgl::impl::opengl
{

class Renderer : virtual public axgl::Renderer, public axgl::impl::ContextHolder
{
  bool initialized_glad_ = false;
  axgl::ptr_t<glfw::Window> window_;

  glm::vec2 viewport_{0.0f};

  std::unique_ptr<::opengl::Texture> screen_texture_;
  std::unique_ptr<::opengl::Texture> depth_texture_;
  std::unique_ptr<::opengl::Framebuffer> screen_framebuffer_;

  renderer::Hdr hdr;
  renderer::Msaa msaa;
  renderer::Blend blend;
  renderer::ShadowMap shadow;

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
      setup_screen_framebuffer(viewport_i);
      if (msaa.enabled) msaa.setup(viewport_i);
      if (blend.enabled) blend.setup(viewport_i, *depth_texture_);
      if (hdr.enabled) hdr.setup(viewport_i);
      if (gui)
      {
        gui->set_size(viewport_i.x, viewport_i.y);
        gui->init();
      }
    }
    msaa.update(viewport_i);
    blend.update(viewport_i, *depth_texture_);
    hdr.update(viewport_i);
    shadow.update();

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
      impl::opengl::renderer::RenderContext render_context{
        .viewport = viewport_f,
        .viewpoint = camera->position,
        .view_matrix = camera->view_matrix(),
        .projection_matrix = camera->projection_matrix(),
        .projection_view_matrix = camera->projection_view_matrix(),
        .inverse_projection_view_matrix
        = glm::inverse(camera->projection_view_matrix()),
        .camera_near = camera->near_clip,
        .camera_far = camera->far_clip,
        .csm_debug_borders = shadow.enable_csm_debug,
      };

      std::unordered_map<
        std::uint64_t, impl::opengl::renderer::RenderComponent*>
        render_components;
      {
        AXGL_PROFILE_SCOPE("Renderer Gather Instances");
        gather_render_components(
          render_context, render_components, realm->entities()
        );
        AXGL_PLOT("Renderer Entity Count", render_context.entity_count);
        AXGL_PLOT("Renderer Component Count", render_context.component_count);
        AXGL_PLOT(
          "Renderer Light Count",
          static_cast<std::int64_t>(render_context.lights.size())
        );
      }
      impl::opengl::renderer::PipelineContext pipeline_context;
      {
        AXGL_PROFILE_SCOPE("Renderer Submit Calls");
        for (auto* render_comp : render_components | std::views::values)
          render_comp->submit_render_function(pipeline_context);
      }

      if (shadow.enable_shadow)
        render_shadow_pass(render_context, pipeline_context);
      render_opaque_pass(render_context, pipeline_context, viewport_i);
      if (blend.enabled)
        render_transparent_pass(render_context, pipeline_context);
    }
    else
    {
      // only clear screen texture if no camera or realm exists
      screen_framebuffer_->use();
      glViewport(0, 0, viewport_i.x, viewport_i.y);
      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT);
    }

    // render screen
    if (hdr.enabled) render_tone_mapping_pass(viewport_i);
    render_gui(gui);
    render_to_screen();
    window_->swap_buffers();
  }

private:
  void setup_screen_framebuffer(const glm::ivec2& viewport)
  {
    screen_texture_ = std::make_unique<::opengl::Texture>();
    screen_texture_->load_texture(
      0, GL_RGBA16F, viewport.x, viewport.y, 0, GL_RGBA, GL_HALF_FLOAT, nullptr
    );
    screen_texture_->set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    screen_texture_->set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    depth_texture_ = std::make_unique<::opengl::Texture>();
    depth_texture_->load_texture(
      0, GL_DEPTH_COMPONENT24, viewport.x, viewport.y, 0, GL_DEPTH_COMPONENT,
      GL_FLOAT, nullptr
    );
    screen_framebuffer_ = std::make_unique<::opengl::Framebuffer>();
    screen_framebuffer_->attach_texture(GL_COLOR_ATTACHMENT0, *screen_texture_);
    screen_framebuffer_->attach_texture(GL_DEPTH_ATTACHMENT, *depth_texture_);
    screen_framebuffer_->check_status_complete(
      "axgl::impl::opengl::Renderer -> screen_framebuffer_"
    );
  }

  void render_shadow_pass(
    impl::opengl::renderer::RenderContext& render_context,
    const impl::opengl::renderer::PipelineContext& pipeline_context
  )
  {
    if (render_context.lights.empty()) return;

    // find the shadow-casting sun light (cascades are sun-only); point/spot
    // keep the single light_pv fallback path in gather_render_components.
    impl::opengl::renderer::LightContext* sun_light_context = nullptr;
    for (auto& lc : render_context.lights)
    {
      if (lc.light && lc.light->type == axgl::Light::Type::kSun
          && lc.light->casts_shadows)
      {
        sun_light_context = &lc;
        break;
      }
    }
    if (!sun_light_context) return;

    sun_light_context->cascades = renderer::ShadowMap::compute_cascades(
      *sun_light_context->light, render_context.inverse_projection_view_matrix,
      render_context.camera_near, render_context.camera_far,
      shadow.shadow_distance
    );

    glViewport(0, 0, shadow.shadow_map_size, shadow.shadow_map_size);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthRange(0.0f, 1.0f);

    AXGL_PROFILE_SCOPE("Render Shadow Map");
    // Render the scene once per cascade: attach layer c, set its light_pv,
    // clear, draw. The depth_only shader reads light_pv from the LightContext
    // so no shadow-pass shader changes are needed.
    for (GLsizei c = 0; c < shadow.shadow_map_cascade_levels; ++c)
    {
      shadow.shadow_framebuffer->use();
      shadow.shadow_framebuffer->attach_texture_layer(
        GL_DEPTH_ATTACHMENT, *shadow.shadow_texture, c
      );
      shadow.shadow_framebuffer->check_status_complete(
        "axgl::impl::opengl::Renderer -> shadow_framebuffer_"
      );
      glClearDepth(1.0);
      glClear(GL_DEPTH_BUFFER_BIT);

      sun_light_context->light_pv = sun_light_context->cascades[c].light_pv;
      for (const auto& render_func : pipeline_context.shadow_pass)
        render_func(*sun_light_context);
    }

    sun_light_context->shadow_map = shadow.shadow_texture.get();
  }

  void render_opaque_pass(
    const impl::opengl::renderer::RenderContext& render_context,
    const impl::opengl::renderer::PipelineContext& pipeline_context,
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
    else screen_framebuffer_->use();
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
      screen_framebuffer_->use_write();
      glBlitFramebuffer(
        0, 0, viewport_i.x, viewport_i.y,                     //
        0, 0, viewport_i.x, viewport_i.y,                     //
        GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST //
      );
    }
  }

  void render_transparent_pass(
    const impl::opengl::renderer::RenderContext& render_context,
    const impl::opengl::renderer::PipelineContext& pipeline_context
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

    screen_framebuffer_->use();
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto& blend_shader = ::opengl::StaticShaders::instance().weighted_blended();
    const auto& quad_vao = ::opengl::StaticVAOs::instance().quad();
    blend.accum_texture->use(GL_TEXTURE0);
    blend.reveal_texture->use(GL_TEXTURE1);
    blend_shader.use_program();
    blend_shader.set_int("accumulation_texture", 0);
    blend_shader.set_int("reveal_texture", 1);
    quad_vao.draw();
  }

  void render_tone_mapping_pass(const glm::ivec2& viewport_i)
  {
    hdr.hdr_framebuffer->use();
    glViewport(0, 0, viewport_i.x, viewport_i.y);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    screen_texture_->use(GL_TEXTURE0);
    auto& screen_shader = ::opengl::StaticShaders::instance().screen();
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
    else screen_framebuffer_->use();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    gui_texture->use(GL_TEXTURE0);
    auto& screen_shader = ::opengl::StaticShaders::instance().screen();
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
    else screen_texture_->use(GL_TEXTURE0);
    auto& screen_shader = ::opengl::StaticShaders::instance().screen();
    screen_shader.use_program();
    screen_shader.set_int("screen", 0);
    screen_shader.set_bool("enable_hdr", false);
    ::opengl::StaticVAOs::instance().quad().draw();
    if (!hdr.enabled) glDisable(GL_FRAMEBUFFER_SRGB);
  }

  // TODO: reimplement without recursion
  void gather_render_components(
    impl::opengl::renderer::RenderContext& render_context,
    std::unordered_map<std::uint64_t, impl::opengl::renderer::RenderComponent*>&
      render_components,
    const axgl::Container<axgl::Entity>& entities,
    const glm::mat4* base_transform_matrix = nullptr
  )
  {
    for (const auto& entity : entities.get())
    {
      if (entity->is_disabled() || entity->is_hidden()) continue;
      ++render_context.entity_count;

      const auto& transform = entity->transform();
      const auto model_matrix = base_transform_matrix //
        ? *base_transform_matrix * transform.model_matrix
        : transform.model_matrix;

      for (const auto& component : entity->components().get())
      {
        if (component->is_disabled() || component->is_hidden()) continue;
        ++render_context.component_count;

        if (auto* render_comp
            = dynamic_cast<impl::opengl::renderer::RenderComponent*>(
              component.get()
            ))
        {
          render_comp->gather_instances(model_matrix);

          const auto id = render_comp->get_id();
          render_components[id] = render_comp;
        }
        else if (const auto* light_comp
                 = dynamic_cast<axgl::impl::component::Light*>(component.get()))
        {
          impl::opengl::renderer::LightContext light_context;
          light_context.light = &light_comp->light;

          render_context.lights.emplace_back(light_context);
        }
      }
      if (!entity->children().empty())
      {
        const auto pivot_matrix = base_transform_matrix //
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
    shadow.enable_shadow = enable_shadow;
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
    return shadow.enable_shadow;
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
};

} // namespace axgl::impl::opengl
