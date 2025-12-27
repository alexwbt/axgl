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

  axgl::ptr_t<axgl::Pointer> pointer_;
  axgl::ptr_t<axgl::Input> activate_;
  axgl::ptr_t<axgl::Input> switch_focus_;

public:
  void set_size(std::uint32_t width, std::uint32_t height) override
  {
    width_ = width;
    height_ = height;
  }

  void init(const axgl::Service::Context& context) override
  {
    pointer_ = axgl::create_ptr<Pointer>("GUI Cursor", Pointer::Source::kMouseMove);
    activate_ = axgl::create_ptr<Input>("GUI Activate", Input::Source::kKeyEnter);
    switch_focus_ = axgl::create_ptr<Input>("GUI Switch Focus", Input::Source::kKeyTab);
  }

  void update(const axgl::Service::Context& context) override
  {
    const auto gui_service = context.axgl.gui_service();
    const auto input_service = context.axgl.input_service();
#ifdef AXGL_DEBUG
    if (!pointer_ || !activate_ || !switch_focus_)
    {
      SPDLOG_WARN("GUI Page update called before init.");
      return;
    }
#endif
    if (
      input_service->get_cursor_mode() == axgl::InputService::CursorMode::kNormal && pointer_->tick > 0
      && (pointer_->delta.x != 0 || pointer_->delta.y != 0))
    {
      const axgl::gui::Page::InputContext current_context{
        context, *gui_service, *this, nullptr, nullptr, *input_service, *pointer_, *activate_, *switch_focus_};
      for (const auto& element : elements_.get())
        element->on_pointer_move(current_context);
    }
  }

  [[nodiscard]] bool should_render() const override { return elements_.should_render(); }

  [[nodiscard]] glm::ivec2 get_size() const override { return {width_, height_}; }

  [[nodiscard]] axgl::ptr_t<axgl::Pointer> get_pointer() const override { return pointer_; }

  [[nodiscard]] axgl::ptr_t<axgl::Input> get_activate_input() const override { return activate_; }

  [[nodiscard]] axgl::ptr_t<axgl::Input> get_switch_focus_input() const override { return switch_focus_; }

  [[nodiscard]] axgl::Container<axgl::gui::Element>& elements() override { return elements_; }
};

} // namespace axgl::impl::gui
