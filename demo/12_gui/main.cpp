#include "axgl/interface/gui/elements/text_element.hpp"
#include "setup.hpp"

#include <demo_gui/res.hpp>

int main()
{
  axgl::Axgl axgl;
  setup(axgl);

  const auto& gui_service = axgl.gui_service();
  const auto& text_service = axgl.text_service();
  const auto& input_service = axgl.input_service();

  // input
  const auto cursor_pointer = axgl::create_ptr<axgl::Pointer>(
    "Cursor", axgl::Pointer::Source::kMouseMove);
  const auto scroll_pointer = axgl::create_ptr<axgl::Pointer>(
    "GUI Scroll", axgl::Pointer::Source::kScroll);
  const auto scale_input = axgl::create_ptr<axgl::Input>(
    "GUI Scale", axgl::Input::Source::kKeyLeftControl);
  const auto activate_input = axgl::create_ptr<axgl::Input>(
    "GUI Activate", axgl::Input::Source::kMouseButton1);
  const auto focus_switch_input = axgl::create_ptr<axgl::Input>(
    "GUI Focus Next", axgl::Input::Source::kKeyTab);
  const auto focus_activate_input = axgl::create_ptr<axgl::Input>(
    "GUI Focus Next", axgl::Input::Source::kKeyEnter);
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
  page->set_focus_switch_input(focus_switch_input);
  page->set_focus_activate_input(focus_activate_input);
  gui_service->set_main_ui(page);

  // gui_service->create_style("box")
  //   ->set_cursor(axgl::gui::Cursor::kPointer)
  //   ->set_position({10.0f, 10.0f})
  //   ->set_size({300.0f, 300.0f})
  //   ->set_font_size(48.0f)
  //   ->set_fonts({"arial", "noto-tc"});
  // gui_service->create_style("box:hover")->set_opacity(0.8f);
  // gui_service->create_style("box:active")->set_opacity(0.6f);

  {
    using namespace axgl::gui;

    gui_service->create_style("h1")
      ->set_display(Display::kBlock)
      ->set_font_size(48.0f)
      ->set_font_weight(900);
    {
      const auto e = gui_service->create_element_t<TextElement>();
      e->set_text("Hello World");
      e->set_style({"h1"});
      page->elements().add(e);
    }
    {
      const auto e = gui_service->create_element_t<TextElement>();
      e->set_text("This is a axgl gui demo.");
      e->set_style({"p"});
    }
  }

  axgl.run();
  axgl.terminate();
}
