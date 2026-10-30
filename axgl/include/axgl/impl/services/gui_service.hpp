#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/services/gui_service.hpp>

#include <axgl/impl/service_base.hpp>
#include <util/string.hpp>

namespace axgl::impl {

class GuiServiceBase : virtual public axgl::GuiService,
                       public axgl::impl::ServiceBase {
public:
  void initialize() override {
    const auto text_style = create_style("text");

    create_style("header1")
      ->set_base_style(text_style)
      ->set_display(axgl::gui::Display::kBlock)
      ->set_font_size(32.0f)
      ->set_margin(glm::vec4(10.0f));

    create_style("paragraph")
      ->set_base_style(text_style)
      ->set_display(axgl::gui::Display::kBlock)
      ->set_margin(glm::vec4(10.0f));

    create_style("button")
      ->set_base_style(text_style)
      ->set_display(axgl::gui::Display::kInline)
      ->set_padding(glm::vec4(10.0f))
      ->set_margin(glm::vec4(10.0f))
      ->set_color({1.0f, 1.0f, 1.0f, 0.2f});
    create_style("button:hover")
      ->set_cursor(axgl::gui::Cursor::kPointer)
      ->set_color({1.0f, 1.0f, 1.0f, 0.5f});
    create_style("button:active")->set_color({1.0f, 1.0f, 1.0f, 0.8f});

    create_style("input")
      ->set_base_style(text_style)
      ->set_display(axgl::gui::Display::kBlock)
      ->set_padding(glm::vec4(6.0f))
      ->set_margin(glm::vec4(10.0f))
      ->set_color({0.0f, 0.0f, 0.0f, 0.4f});
    create_style("input:hover")
      ->set_cursor(axgl::gui::Cursor::kText)
      ->set_color({0.0f, 0.0f, 0.0f, 0.5f});
    create_style("input:focus")->set_color({0.0f, 0.0f, 0.0f, 0.7f});
  }

protected:
  void set_element_default_style(
    const std::string& type_id, const axgl::ptr_t<axgl::gui::Element>& element
  ) {
    const auto split_result = util::split_string(type_id, ':');
    if (split_result.size() < 2 || split_result[0] != "element") return;

    const auto style = get_style(split_result[1]);
    if (!style) return;

    element->append_style(split_result[1]);
  }
};

} // namespace axgl::impl
