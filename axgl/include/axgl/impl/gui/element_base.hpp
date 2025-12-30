#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/gui/element.hpp>
#include <axgl/interface/gui/style.hpp>

#include <axgl/impl/gui/element_container.hpp>

namespace axgl::impl::gui
{

class ElementBase : virtual public axgl::gui::Element
{
protected:
  std::uint64_t id_ = 0;

  bool focusable_ = false;
  bool focused_ = false;
  bool hovering_ = false;
  bool activated_ = false;

  std::shared_ptr<axgl::gui::Style> style_ = axgl::create_ptr<axgl::gui::Style>();
  std::shared_ptr<axgl::gui::Style> focus_style_ = axgl::create_ptr<axgl::gui::Style>();
  std::shared_ptr<axgl::gui::Style> hover_style_ = axgl::create_ptr<axgl::gui::Style>();
  std::shared_ptr<axgl::gui::Style> active_style_ = axgl::create_ptr<axgl::gui::Style>();

  ElementContainer children_;

public:
  ElementBase()
  {
    focus_style_->with(style_);
    hover_style_->with(style_);
    active_style_->with(style_);
  }

  [[nodiscard]] std::uint64_t get_id() const override { return id_; }
  [[nodiscard]] bool focusable() const override { return focusable_; }
  [[nodiscard]] bool focused() const override { return focused_; }
  [[nodiscard]] bool hovering() const override { return hovering_; }
  [[nodiscard]] bool activated() const override { return activated_; }
  [[nodiscard]] axgl::ptr_t<axgl::gui::Style> style() const override { return style_; }
  [[nodiscard]] axgl::ptr_t<axgl::gui::Style> focus_style() const override { return focus_style_; }
  [[nodiscard]] axgl::ptr_t<axgl::gui::Style> hover_style() const override { return hover_style_; }
  [[nodiscard]] axgl::ptr_t<axgl::gui::Style> active_style() const override { return active_style_; }

  [[nodiscard]] axgl::Container<axgl::gui::Element>& children() override { return children_; }

  [[nodiscard]] glm::vec2 get_position(const axgl::gui::Page::Context& context) const override
  {
    const auto* parent = context.parent;
    const auto* parent_context = context.parent_context;
    glm::vec2 position = current_style()->get_position();
    while (parent_context && parent)
    {
      position += parent->current_style()->get_position();
      parent = parent_context->parent;
      parent_context = parent_context->parent_context;
    }
    return position * context.scale;
  }

  [[nodiscard]] glm::vec2 get_size(const axgl::gui::Page::Context& context) const override
  {
    return current_style()->get_size() * context.scale;
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
      if (!hovering_) on_pointer_enter(context);
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

  void render_children(const axgl::gui::Page::RenderContext& context, const glm::vec4* scissor_rect)
  {
    axgl::gui::Page::RenderContext current_context = context;
    current_context.parent = this;
    current_context.parent_context = &context;
    current_context.parent_rect = scissor_rect;
    for (const auto& child : children_.get())
      child->render(current_context);
  }
};

} // namespace axgl::impl::gui
