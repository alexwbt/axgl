#pragma once

#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

#include <axgl/interface/renderer.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/glfw/window.hpp>
#include <axgl/impl/opengl/render_component.hpp>
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

  bool msaa_ = true;
  GLsizei sample_count_ = 4;

  std::unique_ptr<::opengl::Texture> multisampled_texture_;
  std::unique_ptr<::opengl::Renderbuffer> multisampled_renderbuffer_;
  std::unique_ptr<::opengl::Framebuffer> multisampled_framebuffer_;

  std::unique_ptr<::opengl::Texture> screen_texture_;
  std::unique_ptr<::opengl::Framebuffer> screen_framebuffer_;

  // std::unique_ptr<::opengl::Texture> depth_texture_;
  // std::unique_ptr<::opengl::Texture> accum_texture_;
  // std::unique_ptr<::opengl::Texture> reveal_texture_;
  // std::unique_ptr<::opengl::Framebuffer> blend_framebuffer_;

  // static constexpr glm::vec4 zero_filler_{0.0f};
  // static constexpr glm::vec4 one_filler_{1.0f};

public:
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
    auto* camera = context.axgl.camera_service()->get_camera();

    const glm::ivec2 viewport_i = window_->get_size();
    glViewport(0, 0, viewport_i.x, viewport_i.y);
    if (const auto viewport_f = glm::vec2(viewport_i); viewport_f != viewport_)
    {
      viewport_ = viewport_f;

      if (camera)
      {
        camera->viewport.x = viewport_f.x;
        camera->viewport.y = viewport_f.y;
        camera->update_projection_view_matrix();
      }
      if (gui)
      {
        gui->set_size(viewport_i.x, viewport_i.y);
        gui->init(context);
      }

      //
      // setup opaque pass framebuffer
      //
      multisampled_texture_ = std::make_unique<::opengl::Texture>();
      // render_texture_->load_texture(0, GL_RGBA16F, viewport_i.x, viewport_i.y, 0, GL_RGBA, GL_HALF_FLOAT, nullptr);
      multisampled_texture_->init_multisample_texture(sample_count_, GL_RGB, viewport_i.x, viewport_i.y, GL_TRUE);
      multisampled_texture_->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      multisampled_texture_->set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      multisampled_renderbuffer_ = std::make_unique<::opengl::Renderbuffer>();
      multisampled_renderbuffer_->init_multisample_renderbuffer(
        sample_count_, GL_DEPTH24_STENCIL8, viewport_i.x, viewport_i.y);
      multisampled_framebuffer_ = std::make_unique<::opengl::Framebuffer>();
      multisampled_framebuffer_->attach_texture(GL_COLOR_ATTACHMENT0, *multisampled_texture_);
      multisampled_framebuffer_->attach_renderbuffer(GL_DEPTH_STENCIL_ATTACHMENT, *multisampled_renderbuffer_);
      multisampled_framebuffer_->check_status_complete("renderer_multisampled_framebuffer");

      screen_texture_ = std::make_unique<::opengl::Texture>();
      screen_texture_->load_texture(0, GL_RGB, viewport_i.x, viewport_i.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
      screen_texture_->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      screen_texture_->set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      screen_framebuffer_ = std::make_unique<::opengl::Framebuffer>();
      screen_framebuffer_->attach_texture(GL_COLOR_ATTACHMENT0, *screen_texture_);
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
    }

    if (camera && realm)
    {
      //
      // Collection/Build Stage
      //
      RenderComponent::Context render_context{camera};
      std::unordered_map<std::uint64_t, RenderComponent*> render_components;
      {
        AXGL_PROFILE_SCOPE("Renderer Collection Stage");
        for (const auto& entity : realm->entities().get())
        {
          for (const auto& component : entity->components().get())
          {
            if (auto* render_comp = dynamic_cast<RenderComponent*>(component.get()))
            {
              render_comp->collect(*entity);

              const auto id = render_comp->get_id();
              render_components[id] = render_comp;
            }
            else if (const auto* light_comp = dynamic_cast<axgl::impl::component::Light*>(component.get()))
              render_context.lights.push_back(&light_comp->light);
          }
        }
      }
      {
        AXGL_PROFILE_SCOPE("Renderer Build Stage");
        for (auto* render_comp : render_components | std::views::values)
          render_comp->build(render_context);
      }

      //
      // Opaque Render Pass
      //
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LESS);
      glDepthMask(GL_TRUE);

      multisampled_framebuffer_->use();
      glClearDepth(1.0);
      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      {
        AXGL_PROFILE_SCOPE("Renderer Opaque Pass");
        for (const auto& render_func : render_context.opaque_pass)
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
      multisampled_framebuffer_->use();
      gui_texture->use(GL_TEXTURE0);
      screen_shader.use_program();
      screen_shader.set_int("screen", 0);
      quad_vao.draw();
    }

    //
    // Resolve multisampled buffers
    //
    multisampled_framebuffer_->use_read();
    screen_framebuffer_->use_write();
    glBlitFramebuffer(
      0, 0, viewport_i.x, viewport_i.y, //
      0, 0, viewport_i.x, viewport_i.y, //
      GL_COLOR_BUFFER_BIT, GL_NEAREST   //
    );

    //
    // Render To Screen
    //
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    screen_texture_->use(GL_TEXTURE0);
    screen_shader.use_program();
    screen_shader.set_int("screen", 0);
    quad_vao.draw();

    window_->swap_buffers();
  }

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
  }
};

} // namespace axgl::impl::opengl
