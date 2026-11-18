#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/gui/page.hpp>
#include <axgl/interface/services/gui_service.hpp>

#include "app.hpp"
#include "rebind_field.hpp"

namespace demo_gui {

inline void register_rebind_field(
  const axgl::ptr_t<axgl::GuiService>& gui_service
) {
  gui_service->register_element_t<RebindFieldElement>();
}

inline axgl::ptr_t<axgl::gui::Page> build_input_settings(App& app) {
  const auto page = app.create_page();

  page->elements().add(app.ui.text("Input Settings", "header1"));
  page->elements().add(app.ui.text(
    "Click a field, then press a key to rebind (Esc to cancel).", "paragraph"
  ));

  const auto add_row
    = [page, &app](const char* label, const axgl::ptr_t<axgl::Input>& action) {
        page->elements().add(app.ui.text(label, "paragraph"));

        const auto on_rebind = [action](axgl::InputSource source) {
          action->sources = {source};
        };
        if (const auto field = app.ui.rebind(action, on_rebind))
          page->elements().add(field);
      };

  add_row("Move Forward", app.inputs.move_forward);
  add_row("Move Backward", app.inputs.move_backward);
  add_row("Move Left", app.inputs.move_left);
  add_row("Move Right", app.inputs.move_right);
  add_row("Move Up", app.inputs.move_up);
  add_row("Move Down", app.inputs.move_down);

  page->elements().add(app.ui.button("Back", [&app] {
    app.navigate(Route::kMenu);
  }));

  return page;
}

} // namespace demo_gui
