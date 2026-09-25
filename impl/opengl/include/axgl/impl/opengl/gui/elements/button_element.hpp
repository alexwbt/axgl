#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/gui/elements/button_element.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/opengl/gui/element.hpp>
#include <axgl/impl/opengl/gui/elements/text_element.hpp>

namespace axgl::impl::opengl::gui {

class ButtonElement : virtual public axgl::gui::ButtonElement,
                      public axgl::impl::opengl::gui::TextElement {
  std::function<void(const axgl::gui::Context&)> handler_;

public:
  void on_click(
    std::function<void(const axgl::gui::Context&)> handler
  ) override {
    handler_ = std::move(handler);
  }
};

} // namespace axgl::impl::opengl::gui
