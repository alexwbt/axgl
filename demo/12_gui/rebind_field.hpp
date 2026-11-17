#pragma once

#include <functional>
#include <string>

#include <axgl/common.hpp>
#include <axgl/interface/gui/elements/text_element.hpp>
#include <axgl/interface/input.hpp>

#include <axgl/impl/opengl/gui/elements/text_element.hpp>

#include "actions.hpp"

namespace demo_gui {

class RebindFieldElement : virtual public axgl::gui::TextElement,
                           public axgl::impl::opengl::gui::TextElement {
public:
  using Handler
    = std::function<void(const axgl::gui::Context&, axgl::Input::Source)>;

  static constexpr std::string_view kTypeId = "element:demo_rebind_field";

  void set_source(axgl::Input::Source source) {
    source_ = source;
    update_label();
  }

  [[nodiscard]] axgl::Input::Source get_source() const { return source_; }

  void on_rebind(Handler handler) { handler_ = std::move(handler); }

  void on_activate(const axgl::gui::Context& context) override {
    axgl::impl::opengl::gui::TextElement::on_activate(context);
    listening_ = !listening_;
    update_label();
    context.page->set_should_render(true);
  }

  void on_blur(const axgl::gui::Context& context) override {
    axgl::impl::opengl::gui::TextElement::on_blur(context);
    listening_ = false;
    update_label();
    context.page->set_should_render(true);
  }

  void on_keydown(
    const axgl::gui::Context& context, const axgl::KeyEvent& event
  ) override {
    using enum axgl::Input::Source;
    if (!listening_) return;

    if (event.key == kKeyEscape) {
      listening_ = false;
      update_label();
      context.page->set_should_render(true);
      return;
    }

    source_ = event.key;
    listening_ = false;
    update_label();
    if (handler_) handler_(context, event.key);
    context.page->set_should_render(true);
  }

private:
  axgl::Input::Source source_ = axgl::Input::Source::kKeyA;
  bool listening_ = false;
  Handler handler_;

  void update_label() {
    set_text(listening_ ? "Press a key..." : demo_gui::source_name(source_));
  }
};

} // namespace demo_gui
