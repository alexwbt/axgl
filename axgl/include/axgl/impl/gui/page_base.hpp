#pragma once

#include <axgl/interface/gui/page.hpp>
#include <axgl/interface/services/input_service.hpp>

#include <axgl/impl/gui/element_container.hpp>

namespace axgl::impl::gui
{

class PageBase : virtual public axgl::gui::Page
{
protected:
  std::uint32_t width_ = 0;
  std::uint32_t height_ = 0;
  axgl::impl::gui::ElementContainer elements_;

  float scale_ = 1.0f;
  bool should_render_ = false;
  bool using_cursor_ = false;
  axgl::gui::CursorType cursor_type_ = axgl::gui::CursorType::kNormal;
  axgl::ptr_t<axgl::Pointer> cursor_pointer_;
  axgl::ptr_t<axgl::Pointer> scroll_pointer_;
  axgl::ptr_t<axgl::Input> scale_input_;
  axgl::ptr_t<axgl::Input> activate_input_;
  axgl::ptr_t<axgl::Input> focus_switch_input_;
  axgl::ptr_t<axgl::Input> focus_activate_input_;

public:
  void set_size(std::uint32_t width, std::uint32_t height) override
  {
    width_ = width;
    height_ = height;
  }
  void set_scale(float scale) override { scale_ = scale; }
  void set_should_render(bool should_render) override { should_render_ = should_render; }

  void init(const axgl::Service::Context& context) override { should_render_ = true; }

  void update(const axgl::Service::Context& context) override
  {
    const auto& gui_service = context.axgl.gui_service();
    const bool normal_cursor_mode
      = context.axgl.input_service()->get_cursor_mode() == axgl::InputService::CursorMode::kNormal;
    if (cursor_pointer_ && normal_cursor_mode)
    {
      using_cursor_ = true;
      cursor_type_ = axgl::gui::CursorType::kNormal;
    }
    else if (using_cursor_)
    {
      using_cursor_ = false;
      const axgl::gui::Page::Context current_context{
        context,                     //
        *context.axgl.gui_service(), //
        *this,                       //
        nullptr,                     //
        scale_,                      //
      };
      for (const auto& element : elements_.get())
        if (element->hovering()) element->on_pointer_exit(current_context);
    }

    if (
      normal_cursor_mode && scale_input_ && scroll_pointer_ && scale_input_->tick > 0
      && scroll_pointer_->delta.y != 0.0f)
    {
      should_render_ = true;
      scale_ += scroll_pointer_->delta.y * 0.1f;
      if (scale_ <= 0.1f) scale_ = 0.1f;
    }
    const axgl::gui::Page::Context current_context{
      context,                     //
      *context.axgl.gui_service(), //
      *this,                       //
      nullptr,                     //
      scale_,                      //
    };
    for (const auto& element : elements_.get())
      element->update(current_context);
  }

  void render(const axgl::Service::Context& context) override { should_render_ = false; }

  [[nodiscard]] bool should_render() const override { return should_render_; }
  [[nodiscard]] glm::ivec2 get_size() const override { return {width_, height_}; }
  [[nodiscard]] axgl::Container<axgl::gui::Element>& elements() override { return elements_; }

  void set_cursor_type(axgl::gui::CursorType cursor_type) override { cursor_type_ = cursor_type; }
  [[nodiscard]] axgl::gui::CursorType get_cursor_type() override { return cursor_type_; }

  void set_cursor_pointer(axgl::ptr_t<axgl::Pointer> cursor) override { cursor_pointer_ = std::move(cursor); }
  void set_scroll_pointer(axgl::ptr_t<axgl::Pointer> scroll) override { scroll_pointer_ = std::move(scroll); }
  void set_scale_input(axgl::ptr_t<axgl::Input> scale) override { scale_input_ = std::move(scale); }
  void set_activate_input(axgl::ptr_t<axgl::Input> activate) override { activate_input_ = std::move(activate); }
  void set_focus_switch_input(axgl::ptr_t<axgl::Input> focus_switch) override
  {
    focus_switch_input_ = std::move(focus_switch);
  }
  void set_focus_activate_input(axgl::ptr_t<axgl::Input> focus_activate) override
  {
    focus_activate_input_ = std::move(focus_activate);
  }
  [[nodiscard]] axgl::ptr_t<axgl::Pointer> get_cursor_pointer() const override { return cursor_pointer_; }
  [[nodiscard]] axgl::ptr_t<axgl::Pointer> get_scroll_pointer() const override { return scroll_pointer_; }
  [[nodiscard]] axgl::ptr_t<axgl::Input> get_scale_input() const override { return scale_input_; }
  [[nodiscard]] axgl::ptr_t<axgl::Input> get_activate_input() const override { return activate_input_; }
  [[nodiscard]] axgl::ptr_t<axgl::Input> get_focus_switch_input() const override { return focus_switch_input_; }
  [[nodiscard]] axgl::ptr_t<axgl::Input> get_focus_activate_input() const override { return focus_activate_input_; }
};

} // namespace axgl::impl::gui
