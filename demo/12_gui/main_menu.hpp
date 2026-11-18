#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/gui/page.hpp>

#include "app.hpp"

namespace demo_gui {

inline axgl::ptr_t<axgl::gui::Page> build_main_menu(App& app) {
  const auto page = app.create_page();

  page->elements().add(app.ui.text("Main Menu", "header1"));
  page->elements().add(app.ui.text(
    "A CSS-inspired GUI demo. Navigate with the mouse or Tab + Enter.",
    "paragraph"
  ));

  page->elements().add(app.ui.button("Play", [] {
    AXGL_LOG_INFO("play pressed");
  }));

  page->elements().add(app.ui.button("Input Settings", [&app] {
    app.navigate(Route::kInputSettings);
  }));

  page->elements().add(app.ui.button("Display Settings", [&app] {
    app.navigate(Route::kDisplaySettings);
  }));

  return page;
}

} // namespace demo_gui
