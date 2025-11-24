#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/entity/gui_element.hpp>
#include <axgl/interface/realm.hpp>
#include <axgl/util/string.hpp>

#include <axgl/impl/entity/context_override.hpp>
#include <axgl/impl/opengl/component/mesh.hpp>
#include <axgl/impl/opengl/text.hpp>
#include <axgl/impl/realm_service.hpp>

#include <opengl/static_vaos.hpp>

NAMESPACE_AXGL_IMPL

namespace entity
{

class OpenglGuiElement : virtual public interface::entity::GuiElement, public Entity
{
  class Container final : public ContextOverride
  {
  public:
    void update() override
    {
      if (parent_context_->camera->viewport != camera_.viewport)
      {
        camera_.viewport = parent_context_->camera->viewport;
        camera_.set_projection_view_matrix(glm::ortho(camera_.viewport.x, 0.0f, 0.0f, camera_.viewport.y));
      }
      ContextOverride::update();
    }
  };

  StateProperties props_;

  std::shared_ptr<interface::Entity> container_ = nullptr;
  std::shared_ptr<interface::Entity> background_ = nullptr;
  std::shared_ptr<interface::Entity> content_text_ = nullptr;

public:
  [[nodiscard]] StateProperties* props() override { return &props_; }
  [[nodiscard]] uint32_t down_tick() override { return 0; }
  [[nodiscard]] bool hovering() override { return false; }
  [[nodiscard]] bool focused() override { return false; }

  void on_create() override
  {
    ZoneScopedN("OpenglGuiElement On Create");

    Entity::on_create();

    container_ = std::make_shared<Container>();
    add_child(container_);

    update_content_text();
    update_background();
    update_props();
  }

  void update_props() override
  {
    if (background_)
    {
      background_->transform()->scale = glm::vec3(props_.size, 1);
      background_->transform()->position = glm::vec3(0, 0, -0.9f);
      background_->update_model_matrix();
    }
    transform()->position = glm::vec3(props_.origin + props_.offset, 0);
    update_model_matrix();
  }

private:
  void update_content_text()
  {
    if (content_text_)
    {
      remove_child(content_text_);
      content_text_ = nullptr;
    }

    if (props_.content.empty())
      return;

    const auto context = get_context();
    const auto text_service = context->axgl->get_service<OpenglTextService>("text");

    const auto font = util::split(props_.font, ',');
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
    options.size = static_cast<uint32_t>(props_.font_size);
    content_text_ = text_service->create_text(props_.content, font, options);

    const auto text_mesh = content_text_->get_component_t<interface::component::Mesh>();
    text_mesh->get_material()->set_color(props_.fg_color);

    container_->add_child(content_text_);
  }

  void update_background()
  {
    if (!background_)
    {
      const auto context = get_context();
      const auto realm_service = context->axgl->realm_service();
      const auto renderer_service = context->axgl->renderer_service();

      const auto material = renderer_service->create_material("2d");
      const auto mesh = realm_service->create_component_impl<interface::component::Mesh, component::OpenglMesh>();
      mesh->replace_vao(opengl::StaticVAOs::instance().quad());
      mesh->set_material(material);

      background_ = realm_service->create_entity<interface::Entity>();
      background_->add_component(mesh);
      container_->add_child(background_);
    }
    background_->get_component_t<interface::component::Mesh>()->get_material()->set_color(props_.bg_color);
  }
};

} // namespace entity

NAMESPACE_AXGL_IMPL_END
