#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <axgl/common.hpp>
#include <axgl/util/string.hpp>
#include <axgl/interface/component/gui_element.hpp>

#include <axgl/impl/realm_service.hpp>

#include <axgl/impl/opengl/text.hpp>
#include <axgl/impl/opengl/component/mesh.hpp>

#include <opengl/static_vaos.hpp>

NAMESPACE_AXGL_IMPL

namespace component
{
  class OpenglGuiElement : public interface::component::GuiElement
  {
  private:
    std::shared_ptr<interface::RealmService> realm_service_;
    std::shared_ptr<interface::RendererService> renderer_service_;

    std::shared_ptr<interface::component::Mesh> content_text_ = nullptr;
    std::shared_ptr<interface::component::Mesh> background_ = nullptr;

    interface::Camera camera_;
    interface::RealmContext context_;
    ComponentContainer components_;

  public:
    void on_create() override
    {
      auto context = get_context();
      realm_service_ = context->axgl->realm_service();
      renderer_service_ = context->axgl->renderer_service();

      update_content_text();
      update_background();

      // set context and parents of the newly created components
      use_context(context);
      set_parent(parent_);

      components_.on_create();
    }

    void on_remove() override
    {
      components_.on_remove();
    }

    void tick() override
    {
      components_.tick();
    }

    void update() override
    {
      // update viewport
      auto context = get_context();
      auto renderer = context->realm->get_renderer();
      const auto& viewport = renderer->viewport();
      if (viewport.x != camera_.viewport.x || viewport.y != camera_.viewport.y)
      {
        camera_.viewport.x = viewport.x;
        camera_.viewport.y = viewport.y;
        camera_.set_projection_view_matrix(
          glm::ortho(camera_.viewport.x, 0.0f, 0.0f, camera_.viewport.y));
      }

      // parent position
      auto parent = get_parent();
      parent->scale = glm::vec3(props.size, 1);
      parent->position = glm::vec3(props.origin + props.offset, 0);
      parent->update_model_matrix();

      components_.update();
    }

    void render() override
    {
      components_.render();
    }

    uint32_t down_tick() override { return 0; }
    bool hovering() override { return false; }
    bool focused() override { return false; }

  public:
    void update_content_text()
    {
      if (content_text_)
      {
        components_.remove_component(content_text_);
        content_text_ = nullptr;
      }

      if (props.content.empty())
        return;

      auto context = get_context();
      auto text_service = context->axgl->get_service<OpenglTextService>("text");

      auto font = util::split(props.font, ',');
      for (const auto& f : font)
      {
        if (!text_service->has_font(f))
        {
          auto resource_service = context->axgl->resource_service();
          auto resource_key = "font/" + f + ".ttf";
#ifdef AXGL_DEBUG
          if (!resource_service->has_resource(resource_key))
            throw std::runtime_error("Required font not found: " + resource_key);
#endif
          text_service->load_font(f, resource_service->get_resource(resource_key));
        }
      }
      content_text_ = text_service->create_text(props.content, font,
        { .size = static_cast<uint32_t>(props.font_size) });
      components_.add_component(content_text_);
    }

    void update_background()
    {
      if (!background_)
      {
        auto mesh = realm_service_->create_component_impl<interface::component::Mesh, component::OpenglMesh>();
        mesh->replace_vao(opengl::StaticVAOs::instance().quad());
        background_ = mesh;

        auto material = renderer_service_->create_material("2d");
        background_->set_material(material);
        components_.add_component(background_);
      }
      background_->get_material()->set_color(props.bg_color);
    }

    void use_context(interface::RealmContext* context) override
    {
      if (context)
      {
        context_ = *context;
        context_.camera = &camera_;
        Component::use_context(&context_);
        components_.use_context(&context_);
      }
      else
      {
        Component::use_context(nullptr);
        components_.use_context(nullptr);
      }
    }

    void set_parent(interface::Entity* parent) override
    {
      Component::set_parent(parent);
      components_.set_parent(parent);
    }
  };
}

NAMESPACE_AXGL_IMPL_END
