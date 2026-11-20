#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include <axgl/common.hpp>
#include <axgl/interface/renderer.hpp>
#include <axgl/interface/window.hpp>

#include "app.hpp"

namespace demo_gui {

inline void build_display_settings(App& app) {
  const auto page = app.create_page("/main/settings/display");
  const auto renderer = app.renderer();
  const auto window = app.window();

  page->elements().add(app.ui.text("Display Settings", "header1"));

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

  for (const auto& row : rows)
    page->elements().add(
      app.ui.toggle(row.label, row.value, [renderer, row](bool checked) {
        row.apply(renderer, checked);
      })
    );

  page->elements().add(app.ui.text("Exposure", "paragraph"));
  page->elements().add(app.ui.slider(
    renderer->get_exposure(), 0.1f, 3.0f, [renderer](float value) {
      renderer->set_exposure(value);
    }
  ));

  page->elements().add(app.ui.text("Resolution", "paragraph"));

  const auto current_size = window->get_size();
  std::size_t selected = 3;
  if (current_size.x == 800 && current_size.y == 600) selected = 0;
  else if (current_size.x == 1000 && current_size.y == 800) selected = 1;
  else if (current_size.x == 1280 && current_size.y == 720) selected = 2;

  page->elements().add(app.ui.dropdown(
    {"800x600", "1000x800", "1280x720", "1600x900"},
    selected,
    [window](std::size_t index) {
      switch (index) {
      case 0: window->set_size(800, 600); break;
      case 1: window->set_size(1000, 800); break;
      case 2: window->set_size(1280, 720); break;
      default: window->set_size(1600, 900); break;
      }
    }
  ));

  page->elements().add(app.ui.button("Back", [&app] {
    app.navigate("/main");
  }));
}

} // namespace demo_gui
