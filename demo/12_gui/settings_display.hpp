#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/gui/elements/button_element.hpp>
#include <axgl/interface/gui/elements/dropdown_element.hpp>
#include <axgl/interface/gui/elements/slider_element.hpp>
#include <axgl/interface/gui/elements/text_element.hpp>
#include <axgl/interface/gui/elements/toggle_element.hpp>
#include <axgl/interface/gui/page.hpp>
#include <axgl/interface/renderer.hpp>
#include <axgl/interface/window.hpp>

#include "navigator.hpp"

namespace demo_gui {

inline axgl::ptr_t<axgl::gui::Page> create_display_settings(
  const axgl::ptr_t<axgl::GuiService>& gui_service,
  const axgl::ptr_t<demo_gui::Navigator>& nav,
  const axgl::ptr_t<axgl::Renderer>& renderer,
  const axgl::ptr_t<axgl::Window>& window
) {
  const auto page = gui_service->create_page();

  const auto title
    = gui_service->create_element_t<axgl::gui::TextElement>({"header1"});
  title->set_text("Display Settings");
  page->elements().add(title);

  struct ToggleRow {
    const char* label;
    bool value;
    void (*apply)(const axgl::ptr_t<axgl::Renderer>&, bool);
  };

  const ToggleRow rows[] = {
    {"MSAA",
     renderer->get_enable_msaa(),
     [](const axgl::ptr_t<axgl::Renderer>& r, bool v) {
       r->set_enable_msaa(v);
     }},
    {"HDR",
     renderer->get_enable_hdr(),
     [](const axgl::ptr_t<axgl::Renderer>& r, bool v) {
       r->set_enable_hdr(v);
     }},
    {"SSAO",
     renderer->get_enable_ssao(),
     [](const axgl::ptr_t<axgl::Renderer>& r, bool v) {
       r->set_enable_ssao(v);
     }},
    {"Bloom",
     renderer->get_enable_bloom(),
     [](const axgl::ptr_t<axgl::Renderer>& r, bool v) {
       r->set_enable_bloom(v);
     }},
    {"Shadow",
     renderer->get_enable_shadow(),
     [](const axgl::ptr_t<axgl::Renderer>& r, bool v) {
       r->set_enable_shadow(v);
     }},
    {"Blend",
     renderer->get_enable_blend(),
     [](const axgl::ptr_t<axgl::Renderer>& r, bool v) {
       r->set_enable_blend(v);
     }},
  };

  for (const auto& row : rows) {
    const auto toggle
      = gui_service->create_element_t<axgl::gui::ToggleElement>();
    toggle->set_text(row.label);
    toggle->set_checked(row.value);
    toggle->on_toggle([renderer, row](const axgl::gui::Context&, bool checked) {
      row.apply(renderer, checked);
    });
    page->elements().add(toggle);
  }

  const auto exposure_label
    = gui_service->create_element_t<axgl::gui::TextElement>({"paragraph"});
  exposure_label->set_text("Exposure");
  page->elements().add(exposure_label);

  const auto exposure
    = gui_service->create_element_t<axgl::gui::SliderElement>();
  exposure->set_range(0.1f, 3.0f);
  exposure->set_value(renderer->get_exposure());
  exposure->on_change([renderer](const axgl::gui::Context&, float value) {
    renderer->set_exposure(value);
  });
  page->elements().add(exposure);

  const auto resolution_label
    = gui_service->create_element_t<axgl::gui::TextElement>({"paragraph"});
  resolution_label->set_text("Resolution");
  page->elements().add(resolution_label);

  const auto resolution
    = gui_service->create_element_t<axgl::gui::DropdownElement>();
  resolution->set_options({"800x600", "1000x800", "1280x720", "1600x900"});
  const auto current_size = window->get_size();
  resolution->set_selected(
    current_size.x == 800 && current_size.y == 600      ? 0
      : current_size.x == 1000 && current_size.y == 800 ? 1
      : current_size.x == 1280 && current_size.y == 720 ? 2
                                                        : 3
  );
  resolution->on_change([window](const axgl::gui::Context&, std::size_t index) {
    switch (index) {
    case 0: window->set_size(800, 600); break;
    case 1: window->set_size(1000, 800); break;
    case 2: window->set_size(1280, 720); break;
    default: window->set_size(1600, 900); break;
    }
  });
  page->elements().add(resolution);

  const auto back = gui_service->create_element_t<axgl::gui::ButtonElement>();
  back->set_text("Back");
  back->on_click([nav](const axgl::gui::Context&) {
    nav->show_main_menu();
  });
  page->elements().add(back);

  return page;
}

} // namespace demo_gui
