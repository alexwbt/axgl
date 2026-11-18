#pragma once

#include <cstddef>
#include <functional>
#include <string>
#include <utility>
#include <vector>

#include <axgl/common.hpp>
#include <axgl/interface/gui/elements/button_element.hpp>
#include <axgl/interface/gui/elements/dropdown_element.hpp>
#include <axgl/interface/gui/elements/slider_element.hpp>
#include <axgl/interface/gui/elements/text_element.hpp>
#include <axgl/interface/gui/elements/toggle_element.hpp>
#include <axgl/interface/services/gui_service.hpp>

#include "rebind_field.hpp"

namespace demo_gui {

struct Ui {
  using Context = axgl::gui::Context;

  axgl::ptr_t<axgl::GuiService> gui;

  static std::vector<std::string> style_list(const std::string& style) {
    if (style.empty()) return {};
    return {style};
  }

  axgl::ptr_t<axgl::gui::Element> text(
    const std::string& value, const std::string& style = {}
  ) {
    const auto element
      = gui->create_element_t<axgl::gui::TextElement>(style_list(style));
    element->set_text(value);
    return element;
  }

  axgl::ptr_t<axgl::gui::Element> box(
    const std::vector<axgl::ptr_t<axgl::gui::Element>>& children = {},
    const std::string& style = {}
  ) {
    const auto element = gui->create_element();
    if (!style.empty()) element->set_style({style});
    for (const auto& child : children)
      element->children().add(child);
    return element;
  }

  axgl::ptr_t<axgl::gui::Element> button(
    const std::string& label, std::function<void()> on_click
  ) {
    const auto element = gui->create_element_t<axgl::gui::ButtonElement>();
    element->set_text(label);
    element->on_click([handler = std::move(on_click)](const Context&) {
      handler();
    });
    return element;
  }

  axgl::ptr_t<axgl::gui::Element> toggle(
    const std::string& label, bool checked, std::function<void(bool)> on_toggle
  ) {
    const auto element = gui->create_element_t<axgl::gui::ToggleElement>();
    element->set_text(label);
    element->set_checked(checked);
    element->on_toggle([handler
                        = std::move(on_toggle)](const Context&, bool value) {
      handler(value);
    });
    return element;
  }

  axgl::ptr_t<axgl::gui::Element> slider(
    float value, float min, float max, std::function<void(float)> on_change
  ) {
    const auto element = gui->create_element_t<axgl::gui::SliderElement>();
    element->set_range(min, max);
    element->set_value(value);
    element->on_change(
      [handler = std::move(on_change)](const Context&, float new_value) {
        handler(new_value);
      }
    );
    return element;
  }

  axgl::ptr_t<axgl::gui::Element> dropdown(
    const std::vector<std::string>& options,
    std::size_t selected,
    std::function<void(std::size_t)> on_change
  ) {
    const auto element = gui->create_element_t<axgl::gui::DropdownElement>();
    element->set_options(options);
    element->set_selected(selected);
    element->on_change(
      [handler = std::move(on_change)](const Context&, std::size_t index) {
        handler(index);
      }
    );
    return element;
  }

  axgl::ptr_t<axgl::gui::Element> rebind(
    const axgl::ptr_t<axgl::Input>& input,
    std::function<void(axgl::InputSource)> on_rebind
  ) {
    if (!input || input->sources.empty()) return nullptr;

    const auto element = gui->create_element_t<RebindFieldElement>();
    element->set_source(input->sources.front());
    element->on_rebind([handler = std::move(on_rebind)](
                         const Context&, axgl::InputSource source
                       ) {
      handler(source);
    });
    return element;
  }
};

} // namespace demo_gui
