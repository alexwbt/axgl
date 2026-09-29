#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/gui/elements/button_element.hpp>
#include <axgl/interface/gui/elements/text_element.hpp>
#include <axgl/interface/gui/page.hpp>
#include <axgl/interface/services/gui_service.hpp>

inline void setup_page(
  const axgl::ptr_t<axgl::GuiService>& gui_service,
  const axgl::ptr_t<axgl::gui::Page>& page
) {
  using namespace axgl::gui;

  gui_service->create_style("text")->set_fonts({"arial", "noto-tc"});

  gui_service->create_style("h1")
    ->set_display(Display::kBlock)
    ->set_font_size(32.0f)
    ->set_margin(glm::vec4(10.0f));

  gui_service->create_style("p")
    ->set_display(Display::kBlock)
    ->set_margin(glm::vec4(10.0f));

  gui_service->create_style("button")
    ->set_display(Display::kInline)
    ->set_padding(glm::vec4(10.0f))
    ->set_margin(glm::vec4(10.0f))
    ->set_color({1.0f, 1.0f, 1.0f, 0.2f});
  gui_service->create_style("button:hover")
    ->set_cursor(axgl::gui::Cursor::kPointer)
    ->set_color({1.0f, 1.0f, 1.0f, 0.5f});
  gui_service->create_style("button:active")
    ->set_color({1.0f, 1.0f, 1.0f, 0.8f});

  // title
  {
    const auto e = gui_service->create_element_t<TextElement>();
    e->set_text("Hello World");
    e->set_style({"text", "h1"});
    page->elements().add(e);
  }

  // paragraph
  {
    const auto e = gui_service->create_element_t<TextElement>();
    e->set_text(
      "The GUI service is a small, CSS-inspired layer built on top of the "
      "service architecture. It is made of a GuiService, a tree of Page and "
      "Element objects, and a named Style system with hover, active, and "
      "focus pseudo-classes. Named styles are defined once, referenced by "
      "elements by name, and combined into a per-element computed style, so "
      "inline overrides win and recomputation stays lazy. Pages own the "
      "element list, size, render target, and cursor, scroll, scale, and "
      "focus input bindings, while the service drives their lifecycle each "
      "frame. Built-in Text, Button, and Input elements cover common "
      "widgets, and custom types are added by registering a factory."
    );
    e->set_style({"text", "p"});
    page->elements().add(e);
  }

  // button
  {
    const auto e = gui_service->create_element_t<ButtonElement>();
    e->set_text("Click me!");
    e->set_style({"text", "button"});
    e->on_click([](Context) { AXGL_LOG_INFO("hello world"); });
    page->elements().add(e);
  }
}
