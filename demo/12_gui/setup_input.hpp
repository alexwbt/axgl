#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/gui/page.hpp>
#include <axgl/interface/input.hpp>
#include <axgl/interface/pointer.hpp>
#include <axgl/interface/services/input_service.hpp>

namespace demo_gui {

struct Inputs {
  axgl::ptr_t<axgl::Pointer> cursor;
  axgl::ptr_t<axgl::Pointer> scroll;
  axgl::ptr_t<axgl::Input> scale;
  axgl::ptr_t<axgl::Input> activate;
  axgl::ptr_t<axgl::Input> focus_switch;
  axgl::ptr_t<axgl::Input> focus_activate;

  axgl::ptr_t<axgl::Input> move_forward;
  axgl::ptr_t<axgl::Input> move_backward;
  axgl::ptr_t<axgl::Input> move_left;
  axgl::ptr_t<axgl::Input> move_right;
  axgl::ptr_t<axgl::Input> move_up;
  axgl::ptr_t<axgl::Input> move_down;
  axgl::ptr_t<axgl::Input> toggle_camera;

  void bind(const axgl::ptr_t<axgl::InputService>& input_service) const {
    input_service->add_pointer(cursor);
    input_service->add_pointer(scroll);
    input_service->add_input(scale);
    input_service->add_input(activate);
    input_service->add_input(focus_switch);
    input_service->add_input(focus_activate);
    input_service->add_input(move_forward);
    input_service->add_input(move_backward);
    input_service->add_input(move_left);
    input_service->add_input(move_right);
    input_service->add_input(move_up);
    input_service->add_input(move_down);
    input_service->add_input(toggle_camera);
  }

  void apply(const axgl::ptr_t<axgl::gui::Page>& page) const {
    page->set_cursor_pointer(cursor);
    page->set_scroll_pointer(scroll);
    page->set_scale_input(scale);
    page->set_activate_input(activate);
    page->set_focus_switch_input(focus_switch);
    page->set_focus_activate_input(focus_activate);
  }
};

inline Inputs create_inputs() {
  Inputs inputs;
  inputs.cursor = axgl::create_ptr<axgl::Pointer>(
    "Cursor", axgl::Pointer::Source::kMouseMove
  );
  inputs.scroll = axgl::create_ptr<axgl::Pointer>(
    "GUI Scroll", axgl::Pointer::Source::kScroll
  );
  inputs.scale = axgl::create_ptr<axgl::Input>(
    "GUI Scale", axgl::InputSource::kKeyLeftControl
  );
  inputs.activate = axgl::create_ptr<axgl::Input>(
    "GUI Activate", axgl::InputSource::kMouseButton1
  );
  inputs.focus_switch = axgl::create_ptr<axgl::Input>(
    "GUI Focus Next", axgl::InputSource::kKeyTab
  );
  inputs.focus_activate = axgl::create_ptr<axgl::Input>(
    "GUI Focus Activate", axgl::InputSource::kKeyEnter
  );

  inputs.move_forward
    = axgl::create_ptr<axgl::Input>("Move Forward", axgl::InputSource::kKeyW);
  inputs.move_backward
    = axgl::create_ptr<axgl::Input>("Move Backward", axgl::InputSource::kKeyS);
  inputs.move_left
    = axgl::create_ptr<axgl::Input>("Move Left", axgl::InputSource::kKeyA);
  inputs.move_right
    = axgl::create_ptr<axgl::Input>("Move Right", axgl::InputSource::kKeyD);
  inputs.move_up
    = axgl::create_ptr<axgl::Input>("Move Up", axgl::InputSource::kKeySpace);
  inputs.move_down = axgl::create_ptr<axgl::Input>(
    "Move Down", axgl::InputSource::kKeyLeftShift
  );
  inputs.toggle_camera = axgl::create_ptr<axgl::Input>(
    "Toggle Camera", axgl::InputSource::kKeyEscape
  );
  return inputs;
}

} // namespace demo_gui
