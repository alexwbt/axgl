#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/component/mesh.hpp>
#include <axgl/impl/realm_service.hpp>

NAMESPACE_AXGL_IMPL

class OpenglGuiElement : public interface::GuiElement
{
private:
  impl::Component comp_impl_;
  std::shared_ptr<interface::Mesh> quad_mesh_;

public:
  // OpenglGuiElement(std::shared_ptr<interface::Mesh> quad_mesh) :
  //   quad_mesh_(quad_mesh)
  // {}

  void add_component(std::shared_ptr<interface::Component> component) override
  {
    auto element = std::dynamic_pointer_cast<interface::GuiElement>(component);
#ifdef AXGL_DEBUG
    if (!element)
      throw std::runtime_error(
        "Invalid child component type. "
        "Children of GuiElement must also be GuiElement."
      );
#endif
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
    // const auto& children = comp_impl_.get_component_vector();
    // return util::to_iterable(children);
  }

  void update() override
  {
    quad_mesh_->scale = glm::vec3(props.size.width, props.size.height, 1);
    quad_mesh_->get_material()->set_color({ props.bg_color.r, props.bg_color.g, props.bg_color.b });

    comp_impl_.update();
  }

  void render() override
  {
    comp_impl_.render();
  }

  uint32_t down_tick() { return 0; }
  bool hovering() { return false; }
  bool focused() { return false; }
};

NAMESPACE_AXGL_IMPL_END
