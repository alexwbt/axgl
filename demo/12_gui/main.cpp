#include "setup_axgl.hpp"
#include "setup_input.hpp"
#include "setup_page.hpp"
#include <demo_gui/res.hpp>

int main() {
  axgl::Axgl axgl;
  setup_axgl(axgl);

  const auto& gui_service = axgl.gui_service();
  const auto& text_service = axgl.text_service();
  const auto& input_service = axgl.input_service();

  // load fonts
  text_service->load_font("arial", demo_gui_res::get("font/arial.ttf"), 0);
  text_service->load_font("noto-tc", demo_gui_res::get("font/noto-tc.ttf"), 0);

  const auto page = gui_service->create_page();
  setup_input(input_service, page);
  setup_page(gui_service, page);
  gui_service->set_main_ui(page);

  axgl.run();
  axgl.terminate();
}
