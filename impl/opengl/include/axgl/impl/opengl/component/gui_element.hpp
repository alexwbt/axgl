#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <axgl/common.hpp>
#include <axgl/impl/opengl/text.hpp>
#include <axgl/impl/opengl/component/mesh.hpp>
#include <axgl/util/string.hpp>

#include <opengl/static_vaos.hpp>

NAMESPACE_AXGL_IMPL

class OpenglGuiElement : public interface::GuiElement
{
private:
  std::shared_ptr<interface::RealmService> realm_service_;
  std::shared_ptr<interface::RendererService> renderer_service_;

  impl::Component comp_impl_;

  uint32_t content_text_id_ = 0;
  uint32_t background_mesh_id_ = 0;
  
public:
  void on_create() override
  {
    auto context = get_context();
    realm_service_ = context->axgl->realm_service();
    renderer_service_ = context->axgl->renderer_service();

    update_content_text();
    update_background_mesh();
  }

  void update() override
  {
    comp_impl_.update();
  }

  void render() override
  {
    auto current_context = get_context();
    auto viewport = glm::vec2(current_context->renderer->viewport());

    interface::RealmContext context(this);
    context.copy(current_context);
    context.pv = glm::ortho(viewport.x, 0.0f, 0.0f, viewport.y);

    comp_impl_.render();
  }

  uint32_t down_tick() override { return 0; }
  bool hovering() override { return false; }
  bool focused() override { return false; }

public:
  void update_content_text()
  {
    if (content_text_id_ > 0)
    {
      comp_impl_.remove_component(content_text_id_);
      content_text_id_ = 0;
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

    auto text = text_service->create_text(props.content, font,
      { .size = static_cast<uint32_t>(props.font_size) });
    content_text_id_ = text->get_id();
    comp_impl_.add_component(text);
  }

  void update_background_mesh()
  {
    std::shared_ptr<OpenglMesh> background_mesh;

    if (background_mesh_id_ == 0)
    {
      background_mesh = realm_service_->create_component_impl<interface::Mesh, OpenglMesh>();
      background_mesh->replace_vao(opengl::StaticVAOs::instance().quad());
      background_mesh_id_ = background_mesh->get_id();
      comp_impl_.add_component(background_mesh);

      auto material = renderer_service_->create_material("2d");
      background_mesh->set_material(material);
    }

    background_mesh = std::dynamic_pointer_cast<OpenglMesh>(
      comp_impl_.get_component(background_mesh_id_));
#ifdef AXGL_DEBUG
    if (!background_mesh)
      throw std::runtime_error("Failed to get background mesh of OpenglGuiElement.");
#endif
    background_mesh->scale = glm::vec3(props.size, 1);
    background_mesh->position = glm::vec3(props.origin + props.offset, 0);
    background_mesh->update_model_matrix();
    background_mesh->get_material()->set_color(props.bg_color);
  }

public:
  void add_component(std::shared_ptr<interface::Component> component) override
  {
    comp_impl_.add_component(std::move(component));
  }

  void remove_component(uint32_t id) override
  {
    comp_impl_.remove_component(id);
  }

  util::Iterable<std::shared_ptr<interface::Component>> get_components() const override
  {
    return comp_impl_.get_components();
  }

  std::shared_ptr<interface::Component> get_component(uint32_t id) const override
  {
    return comp_impl_.get_component(id);
  }

  void add_child(std::shared_ptr<interface::GuiElement> element) override
  {
    add_component(std::move(element));
  }

  void remove_child(uint32_t id) override
  {
    remove_component(id);
  }

  util::Iterable<std::shared_ptr<interface::GuiElement>> get_children() const override
  {
    throw std::runtime_error("Not implemented");
  }
};

class OpenglGuiPage : public OpenglGuiElement
{
public:

};

NAMESPACE_AXGL_IMPL_END
