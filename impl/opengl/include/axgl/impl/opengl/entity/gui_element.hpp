#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <axgl/common.hpp>
#include <axgl/interface/entity/gui_element.hpp>
#include <axgl/util/string.hpp>

#include <axgl/impl/opengl/text.hpp>
#include <axgl/impl/opengl/component/mesh.hpp>

#include <opengl/static_vaos.hpp>

NAMESPACE_AXGL_IMPL

namespace entity
{
  class OpenglGuiElement : public interface::entity::GuiElement
  {
  private:
    std::shared_ptr<interface::RealmService> realm_service_;
    std::shared_ptr<interface::RendererService> renderer_service_;

    impl::Entity entity_impl_;

    std::shared_ptr<interface::Entity> content_text_ = nullptr;
    std::shared_ptr<component::OpenglMesh> background_mesh_ = nullptr;

  public:
    void on_create() override
    {
      auto context = get_context();
      realm_service_ = context->axgl->realm_service();
      renderer_service_ = context->axgl->renderer_service();

      update_content_text();
      update_background_mesh();
    }

    void tick() override
    {
      entity_impl_.tick();
    }

    void update() override
    {
      scale = glm::vec3(props.size, 1);
      position = glm::vec3(props.origin + props.offset, 0);

      entity_impl_.update();

      update_model_matrix();
    }

    void render() override
    {
      entity_impl_.render();
    }

    uint32_t down_tick() override { return 0; }
    bool hovering() override { return false; }
    bool focused() override { return false; }

  public:
    void update_content_text()
    {
      if (content_text_)
      {
        entity_impl_.remove_child(content_text_);
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

      auto text_mesh = text_service->create_text(props.content, font,
        { .size = static_cast<uint32_t>(props.font_size) });
      content_text_ = realm_service_->create_entity<interface::Entity>();
      content_text_->scale = glm::vec3(props.font_size, props.font_size, 1);
      content_text_->update_model_matrix();

      entity_impl_.add_child(content_text_);
    }

    void update_background_mesh()
    {
      if (!background_mesh_)
      {
        background_mesh_ = realm_service_->create_component_impl<interface::component::Mesh, component::OpenglMesh>();
        background_mesh_->replace_vao(opengl::StaticVAOs::instance().quad());
        entity_impl_.add_component(background_mesh_);

        auto material = renderer_service_->create_material("2d");
        background_mesh_->set_material(material);
      }
      background_mesh_->get_material()->set_color(props.bg_color);
    }

  public:
    void add_component(std::shared_ptr<Component> component) override
    {
      entity_impl_.add_component(std::move(component));
    }

    void remove_component(std::shared_ptr<Component> component) override
    {
      entity_impl_.remove_component(std::move(component));
    }

    util::Iterable<std::shared_ptr<interface::Component>> get_components() const override
    {
      return entity_impl_.get_components();
    }

    void add_child(std::shared_ptr<interface::Entity> entity) override
    {
      entity_impl_.add_child(std::move(entity));
    }

    void remove_child(std::shared_ptr<interface::Entity> entity) override
    {
      entity_impl_.remove_child(std::move(entity));
    }

    util::Iterable<std::shared_ptr<interface::Entity>> get_children() const override
    {
      return entity_impl_.get_children();
    }
  };
}

NAMESPACE_AXGL_IMPL_END
