#include "app.hpp"
#include "main_menu.hpp"
#include "settings_display.hpp"
#include "settings_input.hpp"
#include "setup_axgl.hpp"
#include "style_sheet.hpp"

#include <utility>

#include <demo_gui/res.hpp>

int main() {
  axgl::Axgl axgl;

  auto inputs = demo_gui::create_inputs();
  setup_axgl(axgl, inputs);

  demo_gui::App app(axgl, std::move(inputs));

  const auto& text_service = axgl.text_service();
  text_service->load_font("arial", demo_gui_res::get("font/arial.ttf"), 0);
  text_service->load_font("noto-tc", demo_gui_res::get("font/noto-tc.ttf"), 0);

  demo_gui::setup_styles(app.gui());
  demo_gui::register_rebind_field(app.gui());

  demo_gui::build_main_menu(app);
  demo_gui::build_input_settings(app);
  demo_gui::build_display_settings(app);

  app.navigate("/main");

  axgl.run();
  axgl.terminate();
}
