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
  axgl::gui::Property focused_property_{&property_};
  axgl::gui::Property hovering_property_{&property_};
  axgl::gui::Property activated_property_{&property_};
  ElementContainer children_;

  bool focusable_ = false;
  bool focused_ = false;
  bool hovering_ = false;
  bool activated_ = false;

public:
  [[nodiscard]] std::uint64_t get_id() const override { return id_; }
  [[nodiscard]] axgl::gui::Property& property() override
  {
    if (activated_)
      return activated_property_;
    if (hovering_)
      return hovering_property_;
    if (focused_)
      return focused_property_;
    return property_;
  }
  [[nodiscard]] const axgl::gui::Property& property() const override
  {
    if (activated_)
      return activated_property_;
    if (hovering_)
      return hovering_property_;
    if (focused_)
      return focused_property_;
    return property_;
  }
  [[nodiscard]] axgl::gui::Property& property(const State& state) override
  {
    switch (state)
    {
    default:
    case State::kNormal: return property_;
    case State::kFocused: return focused_property_;
    case State::kHovering: return hovering_property_;
    case State::kActivated: return activated_property_;
    }
  }
  [[nodiscard]] const axgl::gui::Property& property(const State& state) const override
  {
    switch (state)
    {
    default:
    case State::kNormal: return property_;
    case State::kFocused: return focused_property_;
    case State::kHovering: return hovering_property_;
    case State::kActivated: return activated_property_;
    }
  }
  [[nodiscard]] axgl::Container<axgl::gui::Element>& children() override { return children_; }
  [[nodiscard]] bool focusable() const override { return focusable_; }
  [[nodiscard]] bool focused() const override { return focused_; }
  [[nodiscard]] bool hovering() const override { return hovering_; }
  [[nodiscard]] bool activated() const override { return activated_; }

  [[nodiscard]] glm::vec2 get_position(const axgl::gui::Page::Context& context) const override
  {
    const auto* parent = context.parent;
    const auto* parent_context = context.parent_context;
    glm::vec2 position = property().get_position();
    while (parent_context && parent)
    {
      position += context.parent->property().get_position();
      parent = parent_context->parent;
      parent_context = parent_context->parent_context;
    }
    return position * context.scale;
  }

  [[nodiscard]] glm::vec2 get_size(const axgl::gui::Page::Context& context) const override
  {
    return property().get_size() * context.scale;
  }

  [[nodiscard]] glm::vec4 get_rect(const axgl::gui::Page::Context& context) const override
  {
    const auto position = get_position(context);
    return {position, position + get_size(context)};
  }

  void update(const axgl::gui::Page::Context& context) override
  {
    const auto rect = get_rect(context);
    if (                                                       //
      const auto& pointer = context.page.get_cursor_pointer(); //
      pointer                                                  //
      && pointer->position.x > rect.x                          //
      && pointer->position.y > rect.y                          //
      && pointer->position.x < rect.z                          //
      && pointer->position.y < rect.w                          //
    )
    {
      if (!hovering_)
        on_pointer_enter(context);
      update_children(context);
    }
    else if (hovering_)
      on_pointer_exit(context);
  }

  void on_pointer_enter(const axgl::gui::Page::Context& context) override
  {
    hovering_ = true;
    context.page.set_should_render(true);
  }

  void on_pointer_exit(const axgl::gui::Page::Context& context) override
  {
    hovering_ = false;
    context.page.set_should_render(true);

    axgl::gui::Page::Context current_context = context;
    current_context.parent = this;
    current_context.parent_context = &context;
    for (const auto& child : children_.get())
      child->on_pointer_exit(current_context);
  }

  void on_activate(const axgl::gui::Page::Context& context) override
  {
    activated_ = true;
    context.page.set_should_render(true);
  }

  void on_deactivate(const axgl::gui::Page::Context& context) override
  {
    activated_ = false;
    context.page.set_should_render(true);
  }

  void on_focus(const axgl::gui::Page::Context& context) override
  {
    focused_ = true;
    context.page.set_should_render(true);
  }

  void on_blur(const axgl::gui::Page::Context& context) override
  {
    focused_ = false;
    context.page.set_should_render(true);
  }

protected:
  void update_children(const axgl::gui::Page::Context& context)
  {
    axgl::gui::Page::Context current_context = context;
    current_context.parent = this;
    current_context.parent_context = &context;
    for (const auto& child : children_.get())
      child->update(current_context);
  }

  void render_children(const axgl::gui::Page::RenderContext& context)
  {
    axgl::gui::Page::RenderContext current_context = context;
    current_context.parent = this;
    current_context.parent_context = &context;
    for (const auto& child : children_.get())
      child->render(current_context);
  }
};

} // namespace axgl::impl::gui
