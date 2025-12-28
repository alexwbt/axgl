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

  bool cursor_ = false;
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
    const auto input_service = context.axgl.input_service();

    if (pointer_)
      input_service->remove_pointer(pointer_->id);
    if (activate_)
      input_service->remove_input(activate_->id);
    if (switch_focus_)
      input_service->remove_input(switch_focus_->id);

    pointer_ = axgl::create_ptr<Pointer>("GUI Cursor", Pointer::Source::kMouseMove);
    activate_ = axgl::create_ptr<Input>("GUI Activate", Input::Source::kKeyEnter);
    switch_focus_ = axgl::create_ptr<Input>("GUI Switch Focus", Input::Source::kKeyTab);

    input_service->add_pointer(pointer_);
    input_service->add_input(activate_);
    input_service->add_input(switch_focus_);
  }

  void update(const axgl::Service::Context& context) override
  {
    if (!pointer_ || !activate_ || !switch_focus_)
      return;
    const auto gui_service = context.axgl.gui_service();
    const auto input_service = context.axgl.input_service();
    if (input_service->get_cursor_mode() == axgl::InputService::CursorMode::kNormal)
    {
      cursor_ = true;
      const axgl::gui::Page::InputContext current_context{
        context, *gui_service, *this, nullptr, nullptr, *input_service, *pointer_, *activate_, *switch_focus_};
      if (pointer_->delta.x != 0 || pointer_->delta.y != 0)
      {
        for (const auto& element : elements_.get())
          element->update(current_context);
      }
    }
    else if (cursor_)
    {
      cursor_ = false;
      const axgl::gui::Page::InputContext current_context{
        context, *gui_service, *this, nullptr, nullptr, *input_service, *pointer_, *activate_, *switch_focus_};
      for (const auto& element : elements_.get())
        if (element->hovering())
          element->on_pointer_exit(current_context);
    }
  }

  [[nodiscard]] bool should_render() const override { return elements_.should_render(); }

  [[nodiscard]] glm::ivec2 get_size() const override { return {width_, height_}; }

  [[nodiscard]] axgl::Container<axgl::gui::Element>& elements() override { return elements_; }
};

} // namespace axgl::impl::gui
