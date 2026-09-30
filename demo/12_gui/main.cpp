#include "setup_axgl.hpp"
#include "setup_input.hpp"
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
  // TODO: add input manager (for input settings later)
  setup_input(input_service, page);
  gui_service->set_main_ui(page);

  // set font style
  gui_service->get_style("text")->set_fonts({"arial", "noto-tc"});

  // title
  const auto title_element
    = gui_service->create_element_t<axgl::gui::TextElement>({"header1"});
  title_element->set_text("Hello World");
  page->elements().add(title_element);

  // paragraph
  const auto paragraph_element
    = gui_service->create_element_t<axgl::gui::TextElement>({"paragraph"});
  paragraph_element->set_text(
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
  page->elements().add(paragraph_element);

  // button
  const auto button_element
    = gui_service->create_element_t<axgl::gui::ButtonElement>();
  button_element->set_text("Click me!");
  button_element->on_click([](axgl::gui::Context) {
    AXGL_LOG_INFO("hello world");
  });
  page->elements().add(button_element);

  axgl.run();
  axgl.terminate();
}
