#pragma once

#include <glm/glm.hpp>

#include <axgl/common.hpp>
#include <axgl/util/string.hpp>
#include <axgl/interface/realm.hpp>
#include <axgl/interface/entity/gui_element.hpp>

#include <axgl/impl/realm_service.hpp>
#include <axgl/impl/opengl/text.hpp>
#include <axgl/impl/opengl/component/mesh.hpp>

#include <opengl/static_vaos.hpp>

NAMESPACE_AXGL_IMPL

namespace entity
{
  class OpenglGuiElement final : virtual public interface::entity::GuiElement, public Entity
  {
    std::shared_ptr<interface::RealmService> realm_service_;
    std::shared_ptr<interface::RendererService> renderer_service_;

    std::shared_ptr<interface::Entity> content_text_ = nullptr;
    std::shared_ptr<interface::component::Mesh> background_ = nullptr;

    void on_create() override
    {
      const auto context = get_context();
      realm_service_ = context->axgl->realm_service();
      renderer_service_ = context->axgl->renderer_service();

      update_content_text();
      update_background();

      components_.on_create();
      children_.on_create();
    }

    void on_remove() override
    {
      components_.on_remove();
      children_.on_remove();
    }

    void tick() override
    {
      components_.tick();
      children_.tick();
    }

    void update() override
    {
      // update models
      transform()->scale = glm::vec3(props.size, 1);
      transform()->position = glm::vec3(props.origin + props.offset, 0);
      update_model_matrix();

      components_.update();
      children_.update();
    }

    void render() override
    {
      components_.render();
      children_.render();
    }

    uint32_t down_tick() override { return 0; }
    bool hovering() override { return false; }
    bool focused() override { return false; }

    void update_content_text()
    {
      if (content_text_)
      {
        remove_child(content_text_);
        content_text_ = nullptr;
      }

      if (props.content.empty())
        return;

      const auto context = get_context();
      const auto text_service = context->axgl->get_service<OpenglTextService>("text");

      const auto font = util::split(props.font, ',');
      for (const auto& f : font)
      {
        if (!text_service->has_font(f))
        {
          const auto resource_service = context->axgl->resource_service();
          auto resource_key = "font/" + f + ".ttf";
#ifdef AXGL_DEBUG
          if (!resource_service->has_resource(resource_key))
            throw std::runtime_error("Required font not found: " + resource_key);
#endif
          text_service->load_font(f, resource_service->get_resource(resource_key));
        }
      }

      opengl::TextOptions options;
      options.size = static_cast<uint32_t>(props.font_size);
      content_text_ = text_service->create_text(props.content, font, options);

      const auto text_mesh = content_text_->get_component_t<interface::component::Mesh>();
      text_mesh->get_material()->set_color(props.fg_color);

      add_child(content_text_);
    }

    void update_background()
    {
      if (!background_)
      {
        const auto mesh = realm_service_->create_component_impl<interface::component::Mesh, component::OpenglMesh>();
        mesh->replace_vao(opengl::StaticVAOs::instance().quad());
        background_ = mesh;

        const auto material = renderer_service_->create_material("2d");
        background_->set_material(material);

        add_component(background_);
      }
      background_->get_material()->set_color(props.bg_color);
    }
  };
}

NAMESPACE_AXGL_IMPL_END
