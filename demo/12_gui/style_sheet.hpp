#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/services/gui_service.hpp>

namespace demo_gui {

inline void setup_styles(const axgl::ptr_t<axgl::GuiService>& gui_service) {
  gui_service->get_style("text")->set_fonts({"arial", "noto-tc"});

  if (const auto input_style = gui_service->get_style("input"))
    gui_service->create_style("demo_rebind_field")->set_base_style(input_style);
}

} // namespace demo_gui
