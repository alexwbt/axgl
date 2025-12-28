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
#include <opengl/static_shaders.hpp>
#include <opengl/static_vaos.hpp>

namespace axgl::impl::opengl
{

class Renderer : public axgl::Renderer
{
  bool initialized_glad_ = false;
  axgl::ptr_t<glfw::Window> window_;

  std::vector<const axgl::Light*> lights_;

  std::unique_ptr<::opengl::Texture> opaque_texture_;
  std::unique_ptr<::opengl::Texture> depth_texture_;
  std::unique_ptr<::opengl::Texture> accum_texture_;
  std::unique_ptr<::opengl::Texture> reveal_texture_;
  std::unique_ptr<::opengl::Framebuffer> opaque_framebuffer_;
  std::unique_ptr<::opengl::Framebuffer> blend_framebuffer_;

  const glm::vec4 zero_filler_{0.0f};
  const glm::vec4 one_filler_{1.0f};

public:
  void render(const axgl::Service::Context& context) override
  {
    if (!window_ || !window_->ready())
    {
      SPDLOG_DEBUG("Unable to render: window is not set or not ready.");
      return;
    }

    const auto gui = context.axgl.gui_service()->get_main_ui();
    const auto realm = context.axgl.realm_service()->get_active_realm();
    if (!realm)
    {
      SPDLOG_DEBUG("Unable to render realm: active realm is not set.");
      return;
    }

    auto* camera = context.axgl.camera_service()->get_camera();
    if (!camera)
    {
      SPDLOG_DEBUG("Unable to render realm: camera is not set.");
      return;
    }

    const auto viewport = window_->get_size();
    glViewport(0, 0, viewport.x, viewport.y);

    if (const auto v = glm::vec2(viewport); camera->viewport != v)
    {
      camera->viewport.x = v.x;
      camera->viewport.y = v.y;
      camera->update_projection_view_matrix();

      if (gui)
      {
        gui->set_size(viewport.x, viewport.y);
        gui->init(context);
      }

      //
      // setup opaque pass framebuffer
      //
      opaque_texture_ = std::make_unique<::opengl::Texture>();
      opaque_texture_->load_texture(0, GL_RGBA16F, viewport.x, viewport.y, 0, GL_RGBA, GL_HALF_FLOAT, nullptr);
      opaque_texture_->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      opaque_texture_->set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      depth_texture_ = std::make_unique<::opengl::Texture>();
      depth_texture_->load_texture(
        0, GL_DEPTH_COMPONENT, viewport.x, viewport.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
      opaque_framebuffer_ = std::make_unique<::opengl::Framebuffer>();
      opaque_framebuffer_->attach_texture(GL_COLOR_ATTACHMENT0, *opaque_texture_);
      opaque_framebuffer_->attach_texture(GL_DEPTH_ATTACHMENT, *depth_texture_);
      opaque_framebuffer_->set_draw_buffers({GL_COLOR_ATTACHMENT0});

      //
      // setup blend (transparent) pass framebuffer
      //
      accum_texture_ = std::make_unique<::opengl::Texture>();
      accum_texture_->load_texture(0, GL_RGBA16F, viewport.x, viewport.y, 0, GL_RGBA, GL_HALF_FLOAT, nullptr);
      accum_texture_->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      accum_texture_->set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      reveal_texture_ = std::make_unique<::opengl::Texture>();
      reveal_texture_->load_texture(0, GL_R8, viewport.x, viewport.y, 0, GL_RED, GL_FLOAT, nullptr);
      reveal_texture_->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      reveal_texture_->set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      blend_framebuffer_ = std::make_unique<::opengl::Framebuffer>();
      blend_framebuffer_->attach_texture(GL_COLOR_ATTACHMENT0, *accum_texture_);
      blend_framebuffer_->attach_texture(GL_COLOR_ATTACHMENT1, *reveal_texture_);
      blend_framebuffer_->attach_texture(GL_DEPTH_ATTACHMENT, *depth_texture_);
      blend_framebuffer_->set_draw_buffers({GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1});
    }

    glEnable(GL_MULTISAMPLE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

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
    opaque_framebuffer_->use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    {
      AXGL_PROFILE_SCOPE("Renderer Opaque Pass");
      for (const auto& render_func : render_context.opaque_pass)
        render_func(render_context);
    }
    //
    // Transparent Render Pass
    //
    glDepthMask(GL_FALSE);
    glBlendFunci(0, GL_ONE, GL_ONE);
    glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
    glBlendEquation(GL_FUNC_ADD);

    blend_framebuffer_->use();
    glClearBufferfv(GL_COLOR, 0, &zero_filler_[0]);
    glClearBufferfv(GL_COLOR, 1, &one_filler_[0]);
    {
      AXGL_PROFILE_SCOPE("Renderer Transparent Pass");
      for (const auto& render_func : render_context.blend_pass)
        render_func(render_context);
    }

    //
    // Composite Render Pass
    //
    glDepthFunc(GL_ALWAYS);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    opaque_framebuffer_->use();
    accum_texture_->use(GL_TEXTURE0);
    reveal_texture_->use(GL_TEXTURE1);
    ::opengl::StaticShaders::instance().weighted_blended().use_program();
    ::opengl::StaticVAOs::instance().quad().draw();

    //
    // Render Main GUI
    //
    if (gui)
    {
      if (gui->should_render())
      {
        AXGL_PROFILE_SCOPE("GUI Render");
        gui->render(context);
      }

      const auto gui_texture = axgl::ptr_cast<axgl::impl::opengl::Texture>(gui->get_texture());
#ifdef AXGL_DEBUG
      if (!gui_texture)
        throw std::runtime_error("axgl::impl::opengl::Texture is required to use axgl::impl::opengl::Renderer");
#endif
      opaque_framebuffer_->use();
      gui_texture->use(GL_TEXTURE0);
      ::opengl::StaticShaders::instance().screen().use_program();
      ::opengl::StaticVAOs::instance().quad().draw();
    }

    //
    // Render To Screen
    //
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    opaque_texture_->use(GL_TEXTURE0);
    ::opengl::StaticShaders::instance().screen().use_program();
    ::opengl::StaticVAOs::instance().quad().draw();

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
      SPDLOG_CRITICAL("Failed to initialize GLAD.");
    initialized_glad_ = true;
  }
};

} // namespace axgl::impl::opengl
