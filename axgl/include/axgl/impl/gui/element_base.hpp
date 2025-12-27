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
  bool focusable_ = false;
  axgl::gui::Property property_;
  ElementContainer children_;

private:
  std::uint64_t hover_tick_ = 0;

public:
  [[nodiscard]] std::uint64_t get_id() const override { return id_; }
  [[nodiscard]] bool should_render() const override { return should_render_ || children_.should_render(); }
  [[nodiscard]] bool focusable() const override { return focusable_; }
  [[nodiscard]] axgl::gui::Property& property() override { return property_; }
  [[nodiscard]] axgl::Container<axgl::gui::Element>& children() override { return children_; }

  [[nodiscard]] glm::vec2 get_position(const axgl::gui::Page::Context& context) const override
  {
    const auto parent = dynamic_cast<const axgl::impl::gui::ElementBase*>(context.parent);
    return context.parent_context && parent ? parent->get_position(*context.parent_context) + property_.position
                                            : property_.position;
  }

  [[nodiscard]] glm::vec2 get_size(const axgl::gui::Page::Context& context) const override { return property_.size; }

  [[nodiscard]] bool point_in_rect(const axgl::gui::Page::Context& context, glm::vec2 point) const override
  {
    const auto position = get_position(context);
    const auto size = get_size(context);
    return point.x >= position.x && point.y >= position.y && point.x < position.x + size.x
           && point.y < position.y + size.y;
  }

  void update(const axgl::gui::Page::InputContext& context) override
  {
    if (point_in_rect(context, context.pointer.position))
    {
      if (hover_tick_ == 0)
        on_pointer_enter(context);

      for (const auto& child : children_.get())
        child->update(context);

      ++hover_tick_;
    }
    else
    {
      if (hover_tick_ > 0)
        on_pointer_exit(context);
      hover_tick_ = 0;
    }
  }

protected:
  [[nodiscard]] static glm::mat4 get_model_matrix(const glm::vec2& position, const glm::vec2& size)
  {
    return glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f)) * glm::scale(glm::vec3(size, 1.0f));
  }

  void render_children(const axgl::gui::Page::RenderContext& context) const
  {
    axgl::gui::Page::RenderContext current_context = context;
    current_context.parent = this;
    current_context.parent_context = &context;
    for (const auto& child : children_.get())
      child->render(current_context);
  }
};

} // namespace axgl::impl::gui
