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

  glm::vec2 position_{0.0f};
  glm::vec2 size_{0.0f};
  glm::vec4 rect_{0.0f};
  glm::vec4 scissor_rect_{0.0f};

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

  [[nodiscard]] glm::vec2 get_position() const override { return position_; }
  [[nodiscard]] glm::vec2 get_size() const override { return size_; }
  [[nodiscard]] glm::vec4 get_rect() const override { return rect_; }
  [[nodiscard]] glm::vec4 get_scissor_rect() const override { return scissor_rect_; }

  void update(const axgl::gui::Page::Context& context) override
  {
    size_ = current_style()->get_size() * context.scale;
    position_ = current_style()->get_position() * context.scale;
    if (context.parent) position_ += context.parent->get_position();
    scissor_rect_ = rect_ = {position_, position_ + size_};
    if (context.parent)
    {
      const auto parent_rect = context.parent->get_rect();
      scissor_rect_.x = std::max(scissor_rect_.x, parent_rect.x);
      scissor_rect_.y = std::max(scissor_rect_.y, parent_rect.y);
      scissor_rect_.z = std::min(scissor_rect_.z, parent_rect.z);
      scissor_rect_.w = std::min(scissor_rect_.w, parent_rect.w);
    }

    const auto& active_input = context.page.get_activate_input();
    const auto& pointer = context.page.get_cursor_pointer();
    const bool pointer_in_rect = pointer       //
      && pointer->position.x > scissor_rect_.x //
      && pointer->position.y > scissor_rect_.y //
      && pointer->position.x < scissor_rect_.z //
      && pointer->position.y < scissor_rect_.w //
      ;

    if (!hovering_ && pointer_in_rect) on_pointer_enter(context);
    if (hovering_ && !pointer_in_rect) on_pointer_exit(context);
    if (!activated_ && hovering_ && active_input->tick == 1) on_activate(context);
    if (activated_ && active_input->tick == 0) on_deactivate(context);

    if (hovering_) context.page.set_cursor_type(current_style()->get_cursor());
    update_children(context);
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
    for (const auto& child : children_.get())
      child->update(current_context);
  }

  void render_children(const axgl::gui::Page::RenderContext& context)
  {
    axgl::gui::Page::RenderContext current_context = context;
    current_context.parent = this;
    for (const auto& child : children_.get())
      child->render(current_context);
  }
};

} // namespace axgl::impl::gui
