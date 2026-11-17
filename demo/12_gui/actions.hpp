#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/input.hpp>
#include <axgl/interface/services/input_service.hpp>

namespace demo_gui {

inline std::string source_name(axgl::Input::Source source) {
  using enum axgl::Input::Source;
  switch (source) {
  case kKeySpace: return "Space";
  case kKeyApostrophe: return "Apostrophe";
  case kKeyComma: return "Comma";
  case kKeyMinus: return "Minus";
  case kKeyPeriod: return "Period";
  case kKeySlash: return "Slash";
  case kKey0: return "0";
  case kKey1: return "1";
  case kKey2: return "2";
  case kKey3: return "3";
  case kKey4: return "4";
  case kKey5: return "5";
  case kKey6: return "6";
  case kKey7: return "7";
  case kKey8: return "8";
  case kKey9: return "9";
  case kKeyA: return "A";
  case kKeyB: return "B";
  case kKeyC: return "C";
  case kKeyD: return "D";
  case kKeyE: return "E";
  case kKeyF: return "F";
  case kKeyG: return "G";
  case kKeyH: return "H";
  case kKeyI: return "I";
  case kKeyJ: return "J";
  case kKeyK: return "K";
  case kKeyL: return "L";
  case kKeyM: return "M";
  case kKeyN: return "N";
  case kKeyO: return "O";
  case kKeyP: return "P";
  case kKeyQ: return "Q";
  case kKeyR: return "R";
  case kKeyS: return "S";
  case kKeyT: return "T";
  case kKeyU: return "U";
  case kKeyV: return "V";
  case kKeyW: return "W";
  case kKeyX: return "X";
  case kKeyY: return "Y";
  case kKeyZ: return "Z";
  case kKeyEscape: return "Esc";
  case kKeyEnter: return "Enter";
  case kKeyTab: return "Tab";
  case kKeyBackspace: return "Backspace";
  case kKeyDelete: return "Delete";
  case kKeyRight: return "Right";
  case kKeyLeft: return "Left";
  case kKeyDown: return "Down";
  case kKeyUp: return "Up";
  case kKeyHome: return "Home";
  case kKeyEnd: return "End";
  case kKeyLeftShift: return "Left Shift";
  case kKeyLeftControl: return "Left Ctrl";
  case kKeyLeftAlt: return "Left Alt";
  case kKeyLeftSuper: return "Left Super";
  case kKeyRightShift: return "Right Shift";
  case kKeyRightControl: return "Right Ctrl";
  case kKeyRightAlt: return "Right Alt";
  case kKeyRightSuper: return "Right Super";
  case kMouseButton1: return "Mouse 1";
  case kMouseButton2: return "Mouse 2";
  case kMouseButton3: return "Mouse 3";
  default: return "Key";
  }
}

struct Actions {
  axgl::ptr_t<axgl::Input> move_forward;
  axgl::ptr_t<axgl::Input> move_backward;
  axgl::ptr_t<axgl::Input> move_left;
  axgl::ptr_t<axgl::Input> move_right;
  axgl::ptr_t<axgl::Input> move_up;
  axgl::ptr_t<axgl::Input> move_down;
  axgl::ptr_t<axgl::Input> toggle_camera;

  void bind(const axgl::ptr_t<axgl::InputService>& input_service) const {
    input_service->add_input(move_forward);
    input_service->add_input(move_backward);
    input_service->add_input(move_left);
    input_service->add_input(move_right);
    input_service->add_input(move_up);
    input_service->add_input(move_down);
    input_service->add_input(toggle_camera);
  }
};

inline Actions create_actions() {
  return Actions{
    .move_forward
    = axgl::create_ptr<axgl::Input>("Move Forward", axgl::Input::Source::kKeyW),
    .move_backward = axgl::create_ptr<axgl::Input>(
      "Move Backward", axgl::Input::Source::kKeyS
    ),
    .move_left
    = axgl::create_ptr<axgl::Input>("Move Left", axgl::Input::Source::kKeyA),
    .move_right
    = axgl::create_ptr<axgl::Input>("Move Right", axgl::Input::Source::kKeyD),
    .move_up
    = axgl::create_ptr<axgl::Input>("Move Up", axgl::Input::Source::kKeySpace),
    .move_down = axgl::create_ptr<axgl::Input>(
      "Move Down", axgl::Input::Source::kKeyLeftShift
    ),
    .toggle_camera = axgl::create_ptr<axgl::Input>(
      "Toggle Camera", axgl::Input::Source::kKeyEscape
    ),
  };
}

} // namespace demo_gui
