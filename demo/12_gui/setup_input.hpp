#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/gui/page.hpp>
#include <axgl/interface/services/input_service.hpp>

inline void setup_input(
  const axgl::ptr_t<axgl::InputService>& input_service,
  const axgl::ptr_t<axgl::gui::Page>& page
) {
  // input
  const auto cursor_pointer = axgl::create_ptr<axgl::Pointer>(
    "Cursor", axgl::Pointer::Source::kMouseMove
  );
  const auto scroll_pointer = axgl::create_ptr<axgl::Pointer>(
    "GUI Scroll", axgl::Pointer::Source::kScroll
  );
  const auto scale_input = axgl::create_ptr<axgl::Input>(
    "GUI Scale", axgl::Input::Source::kKeyLeftControl
  );
  const auto activate_input = axgl::create_ptr<axgl::Input>(
    "GUI Activate", axgl::Input::Source::kMouseButton1
  );
  const auto focus_switch_input = axgl::create_ptr<axgl::Input>(
    "GUI Focus Next", axgl::Input::Source::kKeyTab
  );
  const auto focus_activate_input = axgl::create_ptr<axgl::Input>(
    "GUI Focus Next", axgl::Input::Source::kKeyEnter
  );
  input_service->add_pointer(cursor_pointer);
  input_service->add_pointer(scroll_pointer);
  input_service->add_input(scale_input);
  input_service->add_input(activate_input);
  input_service->add_input(focus_switch_input);
  input_service->add_input(focus_activate_input);

  page->set_cursor_pointer(cursor_pointer);
  page->set_scroll_pointer(scroll_pointer);
  page->set_scale_input(scale_input);
  page->set_activate_input(activate_input);
  page->set_focus_activate_input(focus_activate_input);
  page->set_focus_switch_input(focus_switch_input);
}
