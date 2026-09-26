#include "setup.hpp"
#include <demo_gui/res.hpp>

#include <axgl/common/color.hpp>

void init_page(
  const axgl::ptr_t<axgl::GuiService>& gui_service,
  const axgl::ptr_t<axgl::gui::Page>& page
) {
  using namespace axgl::gui;
  using namespace axgl::colors;

  gui_service->create_style("text")->set_fonts({"arial", "noto-tc"});

  gui_service->create_style("h1")
    ->set_display(Display::kBlock)
    ->set_font_size(32.0f)
    ->set_font_weight(700)
    ->set_margin(glm::vec4(10.0f));

  gui_service->create_style("p")
    ->set_display(Display::kBlock)
    ->set_margin(glm::vec4(10.0f));

  // title
  {
    const auto e = gui_service->create_element_t<TextElement>();
    e->set_text("Hello World");
    e->set_style({"text", "h1"})->set_color(kRed);
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
    e->set_style({"text", "p"})->set_color(kBlue);
    page->elements().add(e);
  }
  // button
  {
    const auto e = gui_service->create_element_t<ButtonElement>();
    e->set_text("Click me!");
    e->set_style({"button"});
    page->elements().add(e);
  }
}

int main() {
  axgl::Axgl axgl;
  setup(axgl);

  const auto& gui_service = axgl.gui_service();
  const auto& text_service = axgl.text_service();
  const auto& input_service = axgl.input_service();

  // input
  const auto cursor_pointer = axgl::create_ptr<axgl::Pointer>(
    "Cursor", axgl::Pointer::Source::kMouseMove
  );
  const auto scroll_pointer = axgl::create_ptr<axgl::Pointer>(
    "GUI Scroll", axgl::Pointer::Source::kScroll
  );
  const auto scale_input = axgl::create_ptr<axgl::Input>(
    "GUI Scale", axgl::Input::Source::kKeyLeftControl
  );
  const auto activate_input = axgl::create_ptr<axgl::Input>(
    "GUI Activate", axgl::Input::Source::kMouseButton1
  );
  const auto focus_switch_input = axgl::create_ptr<axgl::Input>(
    "GUI Focus Next", axgl::Input::Source::kKeyTab
  );
  const auto focus_activate_input = axgl::create_ptr<axgl::Input>(
    "GUI Focus Next", axgl::Input::Source::kKeyEnter
  );
  input_service->add_pointer(cursor_pointer);
  input_service->add_pointer(scroll_pointer);
  input_service->add_input(scale_input);
  input_service->add_input(activate_input);
  input_service->add_input(focus_switch_input);
  input_service->add_input(focus_activate_input);

  // load fonts
  text_service->load_font("arial", demo_gui_res::get("font/arial.ttf"), 0);
  text_service->load_font("noto-tc", demo_gui_res::get("font/noto-tc.ttf"), 0);

  const auto page = gui_service->create_page();
  page->set_cursor_pointer(cursor_pointer);
  page->set_scroll_pointer(scroll_pointer);
  page->set_scale_input(scale_input);
  page->set_activate_input(activate_input);
  page->set_focus_activate_input(focus_activate_input);
  page->set_focus_switch_input(focus_switch_input);
  init_page(gui_service, page);
  gui_service->set_main_ui(page);

  axgl.run();
  axgl.terminate();
}
