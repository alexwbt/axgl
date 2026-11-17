#include "actions.hpp"
#include "menu.hpp"
#include "navigator.hpp"
#include "rebind_field.hpp"
#include "settings_display.hpp"
#include "settings_input.hpp"
#include "setup_axgl.hpp"
#include "setup_input.hpp"

#include <axgl/interface/gui/elements/input_element.hpp>
#include <axgl/interface/services/gui_service.hpp>
#include <demo_gui/res.hpp>

int main() {
  axgl::Axgl axgl;

  const auto actions = demo_gui::create_actions();
  setup_axgl(axgl, actions);

  const auto& gui_service = axgl.gui_service();
  const auto& text_service = axgl.text_service();

  // load fonts
  text_service->load_font("arial", demo_gui_res::get("font/arial.ttf"), 0);
  text_service->load_font("noto-tc", demo_gui_res::get("font/noto-tc.ttf"), 0);

  // set font style
  gui_service->get_style("text")->set_fonts({"arial", "noto-tc"});

  // style for the demo-local rebind widget (based on the input field)
  if (const auto input_style = gui_service->get_style("input"))
    gui_service->create_style("demo_rebind_field")->set_base_style(input_style);

  // shared GUI input bindings
  const auto mouse_pointer = axgl::create_ptr<axgl::Pointer>(
    "Cursor", axgl::Pointer::Source::kMouseMove
  );
  auto gui_bindings = demo_gui::create_gui_bindings(mouse_pointer);
  gui_bindings.bind(axgl.input_service());

  // current renderer / window (created in setup_axgl)
  const auto& renderer = axgl.renderer_service()->get_active_renderer();
  const auto& window = renderer->get_window();

  // registry for the demo-local rebind widget
  demo_gui::register_rebind_field(gui_service);

  // navigator
  const auto nav = axgl::create_ptr<demo_gui::Navigator>();
  nav->gui = gui_service;

  // build pages
  nav->display_settings
    = demo_gui::create_display_settings(gui_service, nav, renderer, window);
  nav->input_settings
    = demo_gui::create_input_settings(gui_service, nav, actions);

  gui_bindings.apply(nav->display_settings);
  gui_bindings.apply(nav->input_settings);

  nav->main_menu = demo_gui::create_main_menu(gui_service, nav);
  gui_bindings.apply(nav->main_menu);

  nav->gui->set_main_ui(nav->main_menu);

  axgl.run();
  axgl.terminate();
}
