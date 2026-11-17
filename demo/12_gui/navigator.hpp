#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/gui/page.hpp>
#include <axgl/interface/services/gui_service.hpp>

namespace demo_gui {

struct Navigator {
  axgl::ptr_t<axgl::GuiService> gui;
  axgl::ptr_t<axgl::gui::Page> main_menu;
  axgl::ptr_t<axgl::gui::Page> input_settings;
  axgl::ptr_t<axgl::gui::Page> display_settings;

  void show_main_menu() { gui->set_main_ui(main_menu); }
  void show_input_settings() { gui->set_main_ui(input_settings); }
  void show_display_settings() { gui->set_main_ui(display_settings); }
};

} // namespace demo_gui
