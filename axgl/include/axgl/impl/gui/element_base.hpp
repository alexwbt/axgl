#pragma once

#include <glm/gtx/transform.hpp>

#include <axgl/common.hpp>
#include <axgl/interface/gui/element.hpp>
#include <axgl/interface/gui/property.hpp>

#include <axgl/impl/gui/element_container.hpp>

namespace axgl::impl::gui
{

class ElementBase : virtual public axgl::gui::Element
{
protected:
  std::uint64_t id_ = 0;
  bool should_render_ = false;
  axgl::gui::Property property_;
  ElementContainer children_;

public:
  [[nodiscard]] std::uint64_t get_id() const override { return id_; }
  [[nodiscard]] bool should_render() const override { return should_render_ || children_.should_render(); }
  [[nodiscard]] axgl::gui::Property& property() override { return property_; }
  [[nodiscard]] axgl::Container<axgl::gui::Element>& children() override { return children_; }

protected:
  [[nodiscard]] glm::vec2 get_position(const axgl::gui::Page::Context& context) const
  {
    const auto parent = dynamic_cast<const axgl::impl::gui::ElementBase*>(context.parent);
    return context.parent_context && parent ? parent->get_position(*context.parent_context) + property_.position
                                            : property_.position;
  }

  [[nodiscard]] glm::mat4 get_model_matrix(const glm::vec2& position) const
  {
    return glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f)) * glm::scale(glm::vec3(property_.size, 1.0f));
  }

  void render_children(const axgl::gui::Page::Context& context) const
  {
    axgl::gui::Page::Context current_context = context;
    current_context.parent = this;
    current_context.parent_context = &context;
    for (const auto& child : children_.get())
      child->render(current_context);
  }
};

} // namespace axgl::impl::gui
