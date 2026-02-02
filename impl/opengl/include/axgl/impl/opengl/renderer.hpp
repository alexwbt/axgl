#pragma once

#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

#include <axgl/common.hpp>
#include <axgl/interface/renderer.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/glfw/window.hpp>
#include <axgl/impl/opengl/renderer/render_component.hpp>
#include <axgl/impl/opengl/texture.hpp>

#include <opengl/framebuffer.hpp>
#include <opengl/renderbuffer.hpp>
#include <opengl/static_shaders.hpp>
#include <opengl/static_vaos.hpp>

namespace axgl::impl::opengl
{

class Renderer : public axgl::Renderer
{
  bool initialized_glad_ = false;
  axgl::ptr_t<glfw::Window> window_;

  glm::vec2 viewport_{0.0f};

  bool msaa_ = false;
  GLsizei sample_count_ = 4;

  std::unique_ptr<::opengl::Texture> multisample_texture_;
  std::unique_ptr<::opengl::Renderbuffer> multisample_renderbuffer_;
  std::unique_ptr<::opengl::Framebuffer> multisample_framebuffer_;

  std::unique_ptr<::opengl::Texture> screen_texture_;
  std::unique_ptr<::opengl::Texture> depth_texture_;
  std::unique_ptr<::opengl::Framebuffer> screen_framebuffer_;

  //
  // Blending
  //
  // std::unique_ptr<::opengl::Texture> depth_texture_;
  // std::unique_ptr<::opengl::Texture> accum_texture_;
  // std::unique_ptr<::opengl::Texture> reveal_texture_;
  // std::unique_ptr<::opengl::Framebuffer> blend_framebuffer_;
  // static constexpr glm::vec4 zero_filler_{0.0f};
  // static constexpr glm::vec4 one_filler_{1.0f};

  //
  // Shadow Map
  //
  static constexpr std::uint32_t kShadowMapSize = 1024 * 4;
  // static constexpr std::uint32_t kShadowMapCount = 32;
  std::unique_ptr<::opengl::Texture> shadow_texture_;
  std::unique_ptr<::opengl::Framebuffer> shadow_framebuffer_;

public:
  void set_antialiasing(bool enable) override { msaa_ = enable; }

  void set_sample_count(int sample_count) override { sample_count_ = sample_count; }

  void set_window(axgl::ptr_t<axgl::Window> window) override
  {
    window_ = std::dynamic_pointer_cast<glfw::Window>(std::move(window));
    if (!window_)
#ifdef AXGL_DEBUG
      throw std::runtime_error(
        "The provided window is not a valid GlfwWindow instance. "
        "GlfwWindow is required for OpenglRenderer.");
#else
      return;
#endif

    window_->use();

    // initialize glad
    if (!initialized_glad_ && !gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
      AXGL_LOG_ERROR("Failed to initialize GLAD.");
    initialized_glad_ = true;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    window_->swap_buffers();
  }

  [[nodiscard]] bool get_antialiasing() const override { return msaa_; }

  [[nodiscard]] int get_sample_count() const override { return sample_count_; }

  [[nodiscard]] axgl::ptr_t<axgl::Window> get_window() const override { return window_; }

  void render(const axgl::Service::Context& context) override
  {
    if (!window_ || !window_->ready())
    {
      AXGL_LOG_DEBUG("Unable to render: window is not set or not ready.");
      return;
    }

    // auto& blend_shader = ::opengl::StaticShaders::instance().weighted_blended();
    auto& screen_shader = ::opengl::StaticShaders::instance().screen();
    const auto& quad_vao = ::opengl::StaticVAOs::instance().quad();

    const auto& gui = context.axgl.gui_service()->get_main_ui();
    const auto& realm = context.axgl.realm_service()->get_active_realm();

    const auto viewport_i = window_->get_size();
    const auto viewport_f = glm::vec2(viewport_i);
    if (viewport_ != viewport_f)
    {
      viewport_ = viewport_f;

      if (gui)
      {
        gui->set_size(viewport_i.x, viewport_i.y);
        gui->init(context);
      }

      //
      // setup opaque pass framebuffer
      //
      if (msaa_)
      {
        multisample_texture_ = std::make_unique<::opengl::Texture>();
        multisample_texture_->init_multisample_texture(sample_count_, GL_RGB, viewport_i.x, viewport_i.y, GL_TRUE);
        multisample_texture_->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        multisample_texture_->set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        multisample_renderbuffer_ = std::make_unique<::opengl::Renderbuffer>();
        multisample_renderbuffer_->init_multisample_renderbuffer(
          sample_count_, GL_DEPTH24_STENCIL8, viewport_i.x, viewport_i.y);
        multisample_framebuffer_ = std::make_unique<::opengl::Framebuffer>();
        multisample_framebuffer_->attach_texture(GL_COLOR_ATTACHMENT0, *multisample_texture_);
        multisample_framebuffer_->attach_renderbuffer(GL_DEPTH_STENCIL_ATTACHMENT, *multisample_renderbuffer_);
        multisample_framebuffer_->check_status_complete("renderer_multisample_framebuffer");
      }

      screen_texture_ = std::make_unique<::opengl::Texture>();
      screen_texture_->load_texture(0, GL_RGB, viewport_i.x, viewport_i.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
      screen_texture_->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      screen_texture_->set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      depth_texture_ = std::make_unique<::opengl::Texture>();
      depth_texture_->load_texture(
        0, GL_DEPTH_COMPONENT, viewport_i.x, viewport_i.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
      screen_framebuffer_ = std::make_unique<::opengl::Framebuffer>();
      screen_framebuffer_->attach_texture(GL_COLOR_ATTACHMENT0, *screen_texture_);
      screen_framebuffer_->attach_texture(GL_DEPTH_ATTACHMENT, *depth_texture_);
      screen_framebuffer_->check_status_complete("renderer_screen_framebuffer");

      //
      // setup blend (transparent) pass framebuffer
      //
      // accum_texture_ = std::make_unique<::opengl::Texture>();
      // accum_texture_->load_texture(0, GL_RGBA16F, viewport_i.x, viewport_i.y, 0, GL_RGBA, GL_HALF_FLOAT, nullptr);
      // accum_texture_->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      // accum_texture_->set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      // reveal_texture_ = std::make_unique<::opengl::Texture>();
      // reveal_texture_->load_texture(0, GL_R8, viewport_i.x, viewport_i.y, 0, GL_RED, GL_FLOAT, nullptr);
      // reveal_texture_->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      // reveal_texture_->set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      // blend_framebuffer_ = std::make_unique<::opengl::Framebuffer>();
      // blend_framebuffer_->attach_texture(GL_COLOR_ATTACHMENT0, *accum_texture_);
      // blend_framebuffer_->attach_texture(GL_COLOR_ATTACHMENT1, *reveal_texture_);
      // blend_framebuffer_->attach_texture(GL_DEPTH_ATTACHMENT, *depth_texture_);
      // blend_framebuffer_->set_draw_buffers({GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1});

      //
      // setup shadow map
      //
      shadow_texture_ = std::make_unique<::opengl::Texture>();
      shadow_texture_->load_texture(
        0, GL_DEPTH_COMPONENT, kShadowMapSize, kShadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
      shadow_texture_->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      shadow_texture_->set_parameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      shadow_texture_->set_parameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
      shadow_texture_->set_parameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
      shadow_framebuffer_ = std::make_unique<::opengl::Framebuffer>();
      shadow_framebuffer_->attach_texture(GL_DEPTH_ATTACHMENT, *shadow_texture_);
      // glDrawBuffer(GL_NONE);
      // glReadBuffer(GL_NONE);
    }

    auto* camera = context.axgl.camera_service()->get_camera();
    if (camera && camera->viewport != viewport_f)
    {
      camera->viewport.x = viewport_f.x;
      camera->viewport.y = viewport_f.y;
      camera->update_projection_view_matrix();
    }

    if (camera && realm)
    {
      // render context
      impl::opengl::renderer::RenderContext render_context{
        .viewport = viewport_f,
        .viewpoint = camera->position,
        .view_matrix = camera->view_matrix(),
        .projection_matrix = camera->projection_matrix(),
        .projection_view_matrix = camera->projection_view_matrix(),
      };

      //
      // gather and submit render components
      //
      impl::opengl::renderer::PipelineContext pipeline_context;
      std::unordered_map<std::uint64_t, impl::opengl::renderer::RenderComponent*> render_components;
      {
        AXGL_PROFILE_SCOPE("Renderer Gather Instances");
        for (const auto& entity : realm->entities().get())
        {
          if (entity->is_disabled()) continue;
          for (const auto& component : entity->components().get())
          {
            if (component->is_disabled()) continue;
            if (auto* render_comp = dynamic_cast<impl::opengl::renderer::RenderComponent*>(component.get()))
            {
              render_comp->gather_instances(*entity);

              const auto id = render_comp->get_id();
              render_components[id] = render_comp;
            }
            else if (const auto* light_comp = dynamic_cast<axgl::impl::component::Light*>(component.get()))
            {
              impl::opengl::renderer::LightContext light_context;
              light_context.light = &light_comp->light;
              if (light_context.light->casts_shadows) light_context.light_pv = light_context.light->get_pv_matrix();
              render_context.lights.emplace_back(light_context);
            }
          }
        }
      }
      {
        AXGL_PROFILE_SCOPE("Renderer Submit Calls");
        for (auto* render_comp : render_components | std::views::values)
          render_comp->submit_render_function(pipeline_context);
      }

      //
      // Shadow Map Render Pass
      //
      glViewport(0, 0, kShadowMapSize, kShadowMapSize);
      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LESS);

      shadow_framebuffer_->use();
      glClearDepth(1.0);
      glClear(GL_DEPTH_BUFFER_BIT);
      {
        AXGL_PROFILE_SCOPE("Render Shadow Map");
        for (const auto& render_func : pipeline_context.shadow_pass)
          render_func(render_context.lights[0]);
      }

      //
      // Opaque Render Pass
      //
      glViewport(0, 0, viewport_i.x, viewport_i.y);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LESS);
      glDepthMask(GL_TRUE);
      glDepthRange(0.0f, 1.0f);

      if (msaa_) multisample_framebuffer_->use();
      else screen_framebuffer_->use();
      glClearDepth(1.0);
      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      {
        AXGL_PROFILE_SCOPE("Renderer Opaque Pass");
        for (const auto& render_func : pipeline_context.opaque_pass)
          render_func(render_context);
      }

      //
      // Transparent Render Pass
      //
      // glDepthMask(GL_FALSE);
      // glBlendFunci(0, GL_ONE, GL_ONE);
      // glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
      // glBlendEquation(GL_FUNC_ADD);
      //
      // blend_framebuffer_->use();
      // glClearBufferfv(GL_COLOR, 0, &zero_filler_[0]);
      // glClearBufferfv(GL_COLOR, 1, &one_filler_[0]);
      // {
      //   AXGL_PROFILE_SCOPE("Renderer Transparent Pass");
      //   for (const auto& render_func : render_context.blend_pass)
      //     render_func(render_context);
      // }

      //
      // Composite Render Pass
      //
      // glDepthFunc(GL_ALWAYS);
      // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      // render_framebuffer_->use();
      // accum_texture_->use(GL_TEXTURE0);
      // reveal_texture_->use(GL_TEXTURE1);
      // blend_shader.use_program();
      // blend_shader.set_int("accum", 0);
      // blend_shader.set_int("reveal", 1);
      // quad_vao.draw();
    }

    //
    // Render Main GUI
    //
    if (gui)
    {
      if (gui->should_render())
      {
        AXGL_PROFILE_SCOPE("GUI Render");
        gui->render(context);

        const auto& glfw_window = window_->glfw_window();
        switch (gui->get_cursor_type())
        {
        default:
        case gui::CursorType::kNormal: glfw_window->use_standard_cursor(GLFW_ARROW_CURSOR); break;
        case gui::CursorType::kText: glfw_window->use_standard_cursor(GLFW_IBEAM_CURSOR); break;
        case gui::CursorType::kPointer: glfw_window->use_standard_cursor(GLFW_POINTING_HAND_CURSOR); break;
        case gui::CursorType::kCrosshair: glfw_window->use_standard_cursor(GLFW_CROSSHAIR_CURSOR); break;
        case gui::CursorType::kResizeNS: glfw_window->use_standard_cursor(GLFW_RESIZE_NS_CURSOR); break;
        case gui::CursorType::kResizeEW: glfw_window->use_standard_cursor(GLFW_RESIZE_EW_CURSOR); break;
        case gui::CursorType::kResizeNESW: glfw_window->use_standard_cursor(GLFW_RESIZE_NESW_CURSOR); break;
        case gui::CursorType::kResizeNWSE: glfw_window->use_standard_cursor(GLFW_RESIZE_NWSE_CURSOR); break;
        case gui::CursorType::kResize: glfw_window->use_standard_cursor(GLFW_CROSSHAIR_CURSOR); break;
        case gui::CursorType::kNotAllowed: glfw_window->use_standard_cursor(GLFW_NOT_ALLOWED_CURSOR); break;
        }
      }

      const auto gui_texture = axgl::ptr_cast<axgl::impl::opengl::Texture>(gui->get_texture());
#ifdef AXGL_DEBUG
      if (!gui_texture)
        throw std::runtime_error("axgl::impl::opengl::Texture is required to use axgl::impl::opengl::Renderer");
#endif
      multisample_framebuffer_->use();
      gui_texture->use(GL_TEXTURE0);
      screen_shader.use_program();
      screen_shader.set_int("screen", 0);
      quad_vao.draw();
    }

    //
    // Resolve multisample buffers
    //
    if (msaa_)
    {
      multisample_framebuffer_->use_read();
      screen_framebuffer_->use_write();
      glBlitFramebuffer(
        0, 0, viewport_i.x, viewport_i.y, //
        0, 0, viewport_i.x, viewport_i.y, //
        GL_COLOR_BUFFER_BIT, GL_NEAREST   //
      );
    }

    //
    // Render To Screen
    //
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);

    glEnable(GL_FRAMEBUFFER_SRGB);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    screen_texture_->use(GL_TEXTURE0);
    screen_shader.use_program();
    screen_shader.set_int("screen", 0);
    quad_vao.draw();
    glDisable(GL_FRAMEBUFFER_SRGB);

    window_->swap_buffers();
  }
};

} // namespace axgl::impl::opengl
