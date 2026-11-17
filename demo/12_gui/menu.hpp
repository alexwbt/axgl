#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/gui/elements/button_element.hpp>
#include <axgl/interface/gui/elements/text_element.hpp>
#include <axgl/interface/gui/page.hpp>

#include "navigator.hpp"

namespace demo_gui {

inline axgl::ptr_t<axgl::gui::Page> create_main_menu(
  const axgl::ptr_t<axgl::GuiService>& gui_service,
  const axgl::ptr_t<demo_gui::Navigator>& nav
) {
  const auto page = gui_service->create_page();

  const auto title
    = gui_service->create_element_t<axgl::gui::TextElement>({"header1"});
  title->set_text("Main Menu");
  page->elements().add(title);

  const auto subtitle
    = gui_service->create_element_t<axgl::gui::TextElement>({"paragraph"});
  subtitle->set_text(
    "A CSS-inspired GUI demo. Navigate with the mouse or Tab + Enter."
  );
  page->elements().add(subtitle);

  const auto play = gui_service->create_element_t<axgl::gui::ButtonElement>();
  play->set_text("Play");
  play->on_click([nav](const axgl::gui::Context&) {
    AXGL_LOG_INFO("play pressed");
    nav->show_main_menu();
  });
  page->elements().add(play);

  const auto input_button
    = gui_service->create_element_t<axgl::gui::ButtonElement>();
  input_button->set_text("Input Settings");
  input_button->on_click([nav](const axgl::gui::Context&) {
    nav->show_input_settings();
  });
  page->elements().add(input_button);

  const auto display_button
    = gui_service->create_element_t<axgl::gui::ButtonElement>();
  display_button->set_text("Display Settings");
  display_button->on_click([nav](const axgl::gui::Context&) {
    nav->show_display_settings();
  });
  page->elements().add(display_button);

  return page;
}

} // namespace demo_gui
