#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/gui/page.hpp>
#include <axgl/interface/pointer.hpp>
#include <axgl/interface/services/input_service.hpp>

namespace demo_gui {

struct GuiBindings {
  axgl::ptr_t<axgl::Pointer> cursor;
  axgl::ptr_t<axgl::Pointer> scroll;
  axgl::ptr_t<axgl::Input> scale;
  axgl::ptr_t<axgl::Input> activate;
  axgl::ptr_t<axgl::Input> focus_switch;
  axgl::ptr_t<axgl::Input> focus_activate;

  void bind(const axgl::ptr_t<axgl::InputService>& input_service) {
    input_service->add_pointer(cursor);
    input_service->add_pointer(scroll);
    input_service->add_input(scale);
    input_service->add_input(activate);
    input_service->add_input(focus_switch);
    input_service->add_input(focus_activate);
  }

  void apply(const axgl::ptr_t<axgl::gui::Page>& page) {
    page->set_cursor_pointer(cursor);
    page->set_scroll_pointer(scroll);
    page->set_scale_input(scale);
    page->set_activate_input(activate);
    page->set_focus_switch_input(focus_switch);
    page->set_focus_activate_input(focus_activate);
  }
};

inline GuiBindings create_gui_bindings(
  const axgl::ptr_t<axgl::Pointer>& cursor
) {
  GuiBindings bindings;
  bindings.cursor = cursor;
  bindings.scroll = axgl::create_ptr<axgl::Pointer>(
    "GUI Scroll", axgl::Pointer::Source::kScroll
  );
  bindings.scale = axgl::create_ptr<axgl::Input>(
    "GUI Scale", axgl::InputSource::kKeyLeftControl
  );
  bindings.activate = axgl::create_ptr<axgl::Input>(
    "GUI Activate", axgl::InputSource::kMouseButton1
  );
  bindings.focus_switch = axgl::create_ptr<axgl::Input>(
    "GUI Focus Next", axgl::InputSource::kKeyTab
  );
  bindings.focus_activate = axgl::create_ptr<axgl::Input>(
    "GUI Focus Activate", axgl::InputSource::kKeyEnter
  );
  return bindings;
}

} // namespace demo_gui
