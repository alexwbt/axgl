#pragma once

#include <algorithm>
#include <ranges>

#include <axgl/interface/gui/context.hpp>
#include <axgl/interface/gui/page.hpp>
#include <axgl/interface/services/input_service.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/context_holder.hpp>
#include <axgl/impl/gui/element_container.hpp>
#include <axgl/impl/gui/layouts/block_layout.hpp>

namespace axgl::impl::gui {

class PageBase : virtual public axgl::gui::Page,
                 public axgl::impl::ContextHolder {
protected:
  std::uint32_t width_ = 0;
  std::uint32_t height_ = 0;
  axgl::impl::gui::ElementContainer elements_;
  axgl::ptr_t<axgl::gui::Layout> layout_
    = axgl::create_ptr<axgl::impl::gui::BlockLayout>();

  float scale_ = 1.0f;
  float font_scale_ = 1.0f;
  bool should_render_ = false;
  bool using_cursor_ = false;
  std::uint64_t focused_id_ = 0;
  axgl::gui::Cursor cursor_ = axgl::gui::Cursor::kNormal;
  axgl::ptr_t<axgl::Pointer> cursor_pointer_;
  axgl::ptr_t<axgl::Pointer> scroll_pointer_;
  axgl::ptr_t<axgl::Input> scale_input_;
  axgl::ptr_t<axgl::Input> activate_input_;
  axgl::ptr_t<axgl::Input> focus_switch_input_;
  axgl::ptr_t<axgl::Input> focus_activate_input_;

public:
  void set_size(std::uint32_t width, std::uint32_t height) override {
    width_ = width;
    height_ = height;
  }
  void set_scale(float scale) override { scale_ = scale; }
  void set_should_render(bool should_render) override {
    should_render_ = should_render;
  }

  void init() override {
    const auto& gui_service = axgl_->gui_service();
    const axgl::gui::Context current_context{
      *context_,
      gui_service.get(),
      this,
      nullptr,
      scale_,
      font_scale_,
      glm::mat4(1.0f),
    };

    for (const auto& element : elements_.get())
      element->init(current_context);

    const glm::vec2 size{
      static_cast<float>(width_), static_cast<float>(height_)
    };
    layout_->measure(current_context, elements_, size);
    layout_->arrange(current_context, elements_, {0.0f, 0.0f}, size);

    should_render_ = true;
  }

  void update() override {
    const auto& gui_service = axgl_->gui_service();

    const bool normal_cursor_mode = axgl_->input_service()->get_cursor_mode()
      == axgl::InputService::CursorMode::kNormal;

    // update using_cursor_ and cursor_
    if (cursor_pointer_ && normal_cursor_mode) {
      using_cursor_ = true;
      cursor_ = axgl::gui::Cursor::kNormal;
    } else using_cursor_ = false;

    // update scale_
    if (
      normal_cursor_mode && scale_input_ && scroll_pointer_
      && scale_input_->tick > 0 && scroll_pointer_->delta.y != 0.0f
    ) {
      should_render_ = true;
      scale_ += scroll_pointer_->delta.y * 0.1f;
      if (scale_ <= 0.1f) scale_ = 0.1f;
    }

    // context
    const axgl::gui::Context current_context{
      *context_,
      gui_service.get(),
      this,
      nullptr,
      scale_,
      font_scale_,
      glm::mat4(1.0f),
    };

    update_focus(current_context);

    // update elements
    for (const auto& element : elements_.get())
      element->update(current_context);

    const glm::vec2 size{
      static_cast<float>(width_), static_cast<float>(height_)
    };
    layout_->measure(current_context, elements_, size);
    layout_->arrange(current_context, elements_, {0.0f, 0.0f}, size);
  }

  void render() override { should_render_ = false; }

  [[nodiscard]] bool should_render() const override { return should_render_; }
  [[nodiscard]] std::uint32_t get_width() const override { return width_; }
  [[nodiscard]] std::uint32_t get_height() const override { return height_; }
  [[nodiscard]] axgl::Container<axgl::gui::Element>& elements() override {
    return elements_;
  }
  [[nodiscard]] const axgl::gui::Layout& get_layout() const override {
    return *layout_;
  }
  void set_layout(axgl::ptr_t<axgl::gui::Layout> layout) override {
    layout_ = std::move(layout);
  }

  void set_cursor(axgl::gui::Cursor cursor) override { cursor_ = cursor; }
  [[nodiscard]] axgl::gui::Cursor get_cursor() override { return cursor_; }

  void set_cursor_pointer(axgl::ptr_t<axgl::Pointer> cursor) override {
    cursor_pointer_ = std::move(cursor);
  }
  void set_scroll_pointer(axgl::ptr_t<axgl::Pointer> scroll) override {
    scroll_pointer_ = std::move(scroll);
  }
  void set_scale_input(axgl::ptr_t<axgl::Input> scale) override {
    scale_input_ = std::move(scale);
  }
  void set_activate_input(axgl::ptr_t<axgl::Input> activate) override {
    activate_input_ = std::move(activate);
  }
  void set_focus_switch_input(axgl::ptr_t<axgl::Input> focus_switch) override {
    focus_switch_input_ = std::move(focus_switch);
  }
  void set_focus_activate_input(
    axgl::ptr_t<axgl::Input> focus_activate
  ) override {
    focus_activate_input_ = std::move(focus_activate);
  }
  [[nodiscard]] axgl::ptr_t<axgl::Pointer> get_cursor_pointer() const override {
    return using_cursor_ ? cursor_pointer_ : nullptr;
  }
  [[nodiscard]] axgl::ptr_t<axgl::Pointer> get_scroll_pointer() const override {
    return scroll_pointer_;
  }
  [[nodiscard]] axgl::ptr_t<axgl::Input> get_scale_input() const override {
    return scale_input_;
  }
  [[nodiscard]] axgl::ptr_t<axgl::Input> get_activate_input() const override {
    return activate_input_;
  }
  [[nodiscard]] axgl::ptr_t<axgl::Input>
  get_focus_switch_input() const override {
    return focus_switch_input_;
  }
  [[nodiscard]] axgl::ptr_t<axgl::Input>
  get_focus_activate_input() const override {
    return focus_activate_input_;
  }

protected:
  static void collect_focusable(
    axgl::gui::Element* element, std::vector<axgl::gui::Element*>& focusable
  ) {
    if (element->is_focusable()) focusable.push_back(element);
    for (const auto& child : element->children().get())
      collect_focusable(child.get(), focusable);
  }

  void update_focus(const axgl::gui::Context& context) {
    std::vector<axgl::gui::Element*> focusable;
    for (const auto& element : elements_.get())
      collect_focusable(element.get(), focusable);

    axgl::gui::Element* focused = nullptr;
    for (auto* element : focusable)
      if (element->get_id() == focused_id_) focused = element;
    if (!focused) focused_id_ = 0;

    if (
      focus_switch_input_ && focus_switch_input_->clicked()
      && !focusable.empty()
    ) {
      std::size_t next = 0;
      if (focused) {
        const auto it = std::ranges::find(focusable, focused);
        next = (static_cast<std::size_t>(it - focusable.begin()) + 1)
          % focusable.size();
      }
      if (focused) focused->on_blur(context);
      focused = focusable[next];
      focused_id_ = focused->get_id();
      focused->on_focus(context);
    }

    if (focused && focus_activate_input_ && focus_activate_input_->clicked())
      focused->on_activate(context);
  }
};

} // namespace axgl::impl::gui
