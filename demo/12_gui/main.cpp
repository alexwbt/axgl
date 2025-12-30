#include "setup.hpp"

int main()
{
  axgl::Axgl axgl;
  setup(axgl);

  const auto& gui_service = axgl.gui_service();
  const auto& input_service = axgl.input_service();

  const auto cursor_pointer = axgl::create_ptr<axgl::Pointer>("Cursor", axgl::Pointer::Source::kMouseMove);
  const auto scroll_pointer = axgl::create_ptr<axgl::Pointer>("GUI Scroll", axgl::Pointer::Source::kScroll);
  const auto scale_input = axgl::create_ptr<axgl::Input>("GUI Scale", axgl::Input::Source::kKeyLeftControl);
  const auto activate_input = axgl::create_ptr<axgl::Input>("GUI Activate", axgl::Input::Source::kKeyEnter);
  const auto switch_focus_input = axgl::create_ptr<axgl::Input>("GUI Switch Focus", axgl::Input::Source::kKeyTab);
  input_service->add_pointer(cursor_pointer);
  input_service->add_pointer(scroll_pointer);
  input_service->add_input(scale_input);
  input_service->add_input(activate_input);
  input_service->add_input(switch_focus_input);

  const auto page = gui_service->create_page();
  page->set_cursor_pointer(cursor_pointer);
  page->set_scroll_pointer(scroll_pointer);
  page->set_scale_input(scale_input);
  page->set_activate_input(activate_input);
  page->set_switch_focus_input(switch_focus_input);
  gui_service->set_main_ui(page);

  const auto square_style = axgl::create_ptr<axgl::gui::Style>();
  square_style->set_position({10.0f, 10.0f});
  square_style->set_size({300.0f, 300.0f});

  const auto hover_style = axgl::create_ptr<axgl::gui::Style>();
  hover_style->set_opacity(0.5f);

  {
    const auto element1 = gui_service->create_element();
    page->elements().add(element1);
    element1->style()
      ->with(square_style)                     //
      ->set_color({0.39f, 0.58f, 0.93f, 1.0f}) //
      ;
    element1->hover_style()->with(hover_style);
    {
      const auto element2 = gui_service->create_element();
      element1->children().add(element2);
      element2->style()
        ->with(square_style)                  //
        ->set_color({1.0f, 0.5f, 0.3f, 1.0f}) //
        ;
      element2->hover_style()->with(hover_style);
      {
        const auto element3 = gui_service->create_element();
        element2->children().add(element3);
        element3->style()
          ->with(square_style)                  //
          ->set_position({-20.0f, 10.0f})       //
          ->set_size({300.0f, 10.0f})           //
          ->set_color({0.0f, 0.5f, 0.3f, 1.0f}) //
          ;
        element3->hover_style()->with(hover_style);
      }
    }
  }

  axgl.run();
  axgl.terminate();
}
