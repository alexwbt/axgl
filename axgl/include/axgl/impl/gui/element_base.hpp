#pragma once

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
  axgl::gui::Property property_;
  ElementContainer children_;

  bool focusable_ = false;
  bool hovering_ = false;
  bool focused_ = false;
  bool activated_ = false;

public:
  [[nodiscard]] std::uint64_t get_id() const override { return id_; }
  [[nodiscard]] axgl::gui::Property& property() override { return property_; }
  [[nodiscard]] axgl::Container<axgl::gui::Element>& children() override { return children_; }
  [[nodiscard]] bool focusable() const override { return focusable_; }
  [[nodiscard]] bool hovering() const override { return hovering_; }
  [[nodiscard]] bool focused() const override { return focused_; }
  [[nodiscard]] bool activated() const override { return activated_; }

  [[nodiscard]] glm::vec2 get_position(const axgl::gui::Page::Context& context) const override
  {
    return context.parent_context && context.parent
      ? context.parent->get_position(*context.parent_context) + property_.position
      : property_.position;
  }

  [[nodiscard]] glm::vec4 get_rect(const axgl::gui::Page::Context& context) const override
  {
    const auto position = get_position(context);
    return {position, position + property_.size};
  }

  void update(const axgl::gui::Page::InputContext& context) override
  {
    const auto rect = get_rect(context);
    const auto pointer = glm::vec2(context.pointer.position);

    if (pointer.x > rect.x && pointer.y > rect.y && pointer.x < rect.z && pointer.y < rect.w)
    {
      if (!hovering_)
        on_pointer_enter(context);
      update_children(context);
    }
    else if (hovering_)
      on_pointer_exit(context);
  }

  void on_pointer_enter(const axgl::gui::Page::InputContext& context) override
  {
    hovering_ = true;
    context.page.set_should_render(true);
  }

  void on_pointer_exit(const axgl::gui::Page::InputContext& context) override
  {
    hovering_ = false;
    context.page.set_should_render(true);

    axgl::gui::Page::InputContext current_context = context;
    current_context.parent = this;
    current_context.parent_context = &context;
    for (const auto& child : children_.get())
      child->on_pointer_exit(current_context);
  }

  void on_activate(const axgl::gui::Page::InputContext& context) override
  {
    activated_ = true;
    context.page.set_should_render(true);
  }

  void on_deactivate(const axgl::gui::Page::InputContext& context) override
  {
    activated_ = false;
    context.page.set_should_render(true);
  }

  void on_focus(const axgl::gui::Page::InputContext& context) override
  {
    focused_ = true;
    context.page.set_should_render(true);
  }

  void on_blur(const axgl::gui::Page::InputContext& context) override
  {
    focused_ = false;
    context.page.set_should_render(true);
  }

protected:
  void update_children(const axgl::gui::Page::InputContext& context) const
  {
    axgl::gui::Page::InputContext current_context = context;
    current_context.parent = this;
    current_context.parent_context = &context;
    for (const auto& child : children_.get())
      child->update(current_context);
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
