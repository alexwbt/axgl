#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/gui/elements/button_element.hpp>
#include <axgl/interface/gui/elements/text_element.hpp>
#include <axgl/interface/gui/page.hpp>

#include "actions.hpp"
#include "navigator.hpp"
#include "rebind_field.hpp"

namespace demo_gui {

inline void register_rebind_field(
  const axgl::ptr_t<axgl::GuiService>& gui_service
) {
  gui_service->register_element_t<RebindFieldElement>();
}

inline axgl::ptr_t<axgl::gui::Page> create_input_settings(
  const axgl::ptr_t<axgl::GuiService>& gui_service,
  const axgl::ptr_t<demo_gui::Navigator>& nav,
  const Actions& actions
) {
  const auto page = gui_service->create_page();

  const auto title
    = gui_service->create_element_t<axgl::gui::TextElement>({"header1"});
  title->set_text("Input Settings");
  page->elements().add(title);

  const auto hint
    = gui_service->create_element_t<axgl::gui::TextElement>({"paragraph"});
  hint->set_text("Click a field, then press a key to rebind (Esc to cancel).");
  page->elements().add(hint);

  auto add_row =
    [&](const char* label, const axgl::ptr_t<axgl::Input>& action) {
      const auto label_element
        = gui_service->create_element_t<axgl::gui::TextElement>({"paragraph"});
      label_element->set_text(label);
      page->elements().add(label_element);

      if (!action->sources.empty()) {
        auto field = gui_service->create_element_t<RebindFieldElement>();
        field->set_source(action->sources.front());
        field->on_rebind(
          [action](const axgl::gui::Context&, axgl::Input::Source source) {
            action->sources = {source};
          }
        );
        page->elements().add(field);
      }
    };

  add_row("Move Forward", actions.move_forward);
  add_row("Move Backward", actions.move_backward);
  add_row("Move Left", actions.move_left);
  add_row("Move Right", actions.move_right);
  add_row("Move Up", actions.move_up);
  add_row("Move Down", actions.move_down);

  const auto back = gui_service->create_element_t<axgl::gui::ButtonElement>();
  back->set_text("Back");
  back->on_click([nav](const axgl::gui::Context&) {
    nav->show_main_menu();
  });
  page->elements().add(back);

  return page;
}

} // namespace demo_gui
