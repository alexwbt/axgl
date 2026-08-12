#pragma once

#include <algorithm>
#include <memory>
#include <ranges>

#include <axgl/common.hpp>
#include <axgl/interface/gui/context.hpp>
#include <axgl/interface/gui/element.hpp>
#include <axgl/interface/gui/style.hpp>
#include <axgl/interface/services/gui_service.hpp>

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

  std::unique_ptr<axgl::gui::Style> element_style_
    = std::make_unique<axgl::gui::Style>();
  std::unique_ptr<axgl::gui::Style> computed_style_
    = std::make_unique<axgl::gui::Style>();

  bool update_styles_ = false;
  std::vector<std::string> styles_;
  std::vector<axgl::ptr_t<axgl::gui::Style>> using_styles_;

  ElementContainer children_;

  glm::vec2 position_{0.0f};
  glm::vec2 size_{0.0f};
  glm::vec4 rect_{0.0f};
  glm::vec4 scissor_rect_{0.0f};
  glm::vec2 intrinsic_size_{0.0f};

public:
  [[nodiscard]] std::uint64_t get_id() const override { return id_; }
  [[nodiscard]] glm::vec2 get_position() const override { return position_; }
  [[nodiscard]] glm::vec2 get_size() const override { return size_; }
  [[nodiscard]] glm::vec4 get_rect() const override { return rect_; }
  [[nodiscard]] glm::vec4 get_visible_rect() const override
  {
    return scissor_rect_;
  }
  [[nodiscard]] glm::vec2 get_intrinsic_size() const override
  {
    return intrinsic_size_;
  }
  [[nodiscard]] const axgl::gui::Style& get_computed_style() const override
  {
    return *computed_style_;
  }
  [[nodiscard]] bool is_focusable() const override { return focusable_; }
  [[nodiscard]] bool is_focused() const override { return focused_; }
  [[nodiscard]] bool is_hovering() const override { return hovering_; }
  [[nodiscard]] bool is_activated() const override { return activated_; }

  [[nodiscard]] axgl::gui::Style* style() const override
  {
    return element_style_.get();
  }
  [[nodiscard]] axgl::Container<axgl::gui::Element>& children() override
  {
    return children_;
  }

  void init(const axgl::gui::Context& context) override
  {
    update_styles(context);
    init_children(context);
  }

  void update(const axgl::gui::Context& context) override
  {
    // size_ = computed_style_->get_size() * context.scale;
    // position_ = computed_style_->get_position() * context.scale;
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

    const auto& active_input = context.page->get_activate_input();
    const auto& pointer = context.page->get_cursor_pointer();
    const bool pointer_in_rect = pointer       //
      && pointer->position.x > scissor_rect_.x //
      && pointer->position.y > scissor_rect_.y //
      && pointer->position.x < scissor_rect_.z //
      && pointer->position.y < scissor_rect_.w //
      ;

    if (!hovering_ && pointer_in_rect) on_pointer_enter(context);
    if (hovering_ && !pointer_in_rect) on_pointer_exit(context);
    if (!activated_ && hovering_ && active_input->tick == 1)
      on_activate(context);
    if (activated_ && active_input->tick == 0) on_deactivate(context);
    if (hovering_) context.page->set_cursor(computed_style_->get_cursor());

    update_styles(context);
    update_children(context);
  }

  void on_pointer_enter(const axgl::gui::Context& context) override
  {
    hovering_ = true;
    update_styles_ = true;
    context.page->set_should_render(true);
  }

  void on_pointer_exit(const axgl::gui::Context& context) override
  {
    hovering_ = false;
    update_styles_ = true;
    context.page->set_should_render(true);
  }

  void on_activate(const axgl::gui::Context& context) override
  {
    activated_ = true;
    update_styles_ = true;
    context.page->set_should_render(true);
  }

  void on_deactivate(const axgl::gui::Context& context) override
  {
    activated_ = false;
    update_styles_ = true;
    context.page->set_should_render(true);
  }

  void on_focus(const axgl::gui::Context& context) override
  {
    focused_ = true;
    update_styles_ = true;
    context.page->set_should_render(true);
  }

  void on_blur(const axgl::gui::Context& context) override
  {
    focused_ = false;
    update_styles_ = true;
    context.page->set_should_render(true);
  }

  void set_position(glm::vec2 position) override { position_ = position; }

  void set_size(glm::vec2 size) override { size_ = size; }

  axgl::gui::Style* set_style(const std::vector<std::string>& styles) override
  {
    styles_.clear();
    styles_.insert(styles_.end(), styles.begin(), styles.end());
    update_styles_ = true;
    return element_style_.get();
  }

  void append_style(const std::string& style) override
  {
    update_styles_ = true;
    styles_.emplace_back(style);
  }

  void remove_style(const std::string& style) override
  {
    update_styles_ = true;
    std::erase_if(styles_, [&style](const auto& s) { return s == style; });
  }

protected:
  void init_children(const axgl::gui::Context& context)
  {
    axgl::gui::Context current_context = context;
    current_context.parent = this;
    for (const auto& child : children_.get())
      child->init(current_context);
  }

  void update_children(const axgl::gui::Context& context)
  {
    axgl::gui::Context current_context = context;
    current_context.parent = this;
    for (const auto& child : children_.get())
      child->update(current_context);
  }

  void render_children(const axgl::gui::Context& context)
  {
    axgl::gui::Context current_context = context;
    current_context.parent = this;
    for (const auto& child : children_.get())
      child->render(current_context);
  }

  void update_styles(const axgl::gui::Context& context)
  {
    element_style_->reset_modified();

    if (update_styles_)
    {
      using_styles_.clear();
      using_styles_.reserve(styles_.size() * 4);
      for (const auto& style : styles_)
      {
        set_using_style(context.gui_service, style);
        if (hovering_) set_using_style(context.gui_service, style + ":hover");
        if (activated_) set_using_style(context.gui_service, style + ":active");
        if (focused_) set_using_style(context.gui_service, style + ":focus");
      }
    }
    if (update_styles_ || element_style_->is_modified() //
        || std::ranges::any_of(
          using_styles_, [](const auto& s) { return s->is_modified(); }
        ))
    {
      computed_style_ = std::make_unique<axgl::gui::Style>();
      for (const auto& style : using_styles_)
        style->apply_to(*computed_style_);
      element_style_->apply_to(*computed_style_);
      update_styles_ = false;
    }
  }

private:
  void set_using_style(const GuiService* gui_context, const std::string& name)
  {
    if (const auto& style_ptr = gui_context->get_style(name))
      using_styles_.emplace_back(style_ptr);
  }
};

} // namespace axgl::impl::gui
