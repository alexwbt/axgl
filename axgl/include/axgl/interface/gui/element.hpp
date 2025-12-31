#pragma once

#include <axgl/interface/gui/page.hpp>
#include <axgl/interface/gui/style.hpp>

namespace axgl::gui
{

class Element
{
public:
  virtual ~Element() = default;

  [[nodiscard]] virtual std::uint64_t get_id() const = 0;
  [[nodiscard]] virtual bool focusable() const = 0;
  [[nodiscard]] virtual bool focused() const = 0;
  [[nodiscard]] virtual bool hovering() const = 0;
  [[nodiscard]] virtual bool activated() const = 0;
  [[nodiscard]] virtual axgl::ptr_t<Style> style() const = 0;
  [[nodiscard]] virtual axgl::ptr_t<Style> focus_style() const = 0;
  [[nodiscard]] virtual axgl::ptr_t<Style> hover_style() const = 0;
  [[nodiscard]] virtual axgl::ptr_t<Style> active_style() const = 0;
  [[nodiscard]] virtual axgl::ptr_t<Style> current_style() const
  {
    if (activated()) return active_style();
    if (hovering()) return hover_style();
    if (focused()) return focus_style();
    return style();
  }

  [[nodiscard]] virtual axgl::Container<axgl::gui::Element>& children() = 0;

  [[nodiscard]] virtual glm::vec2 get_position() const = 0;
  [[nodiscard]] virtual glm::vec2 get_size() const = 0;
  [[nodiscard]] virtual glm::vec4 get_rect() const = 0;
  [[nodiscard]] virtual glm::vec4 get_scissor_rect() const = 0;

  virtual void render(const axgl::gui::Page::RenderContext& context) = 0;
  virtual void update(const axgl::gui::Page::Context& context) = 0;

  virtual void on_pointer_enter(const axgl::gui::Page::Context& context) = 0;
  virtual void on_pointer_exit(const axgl::gui::Page::Context& context) = 0;
  virtual void on_activate(const axgl::gui::Page::Context& context) = 0;
  virtual void on_deactivate(const axgl::gui::Page::Context& context) = 0;
  virtual void on_focus(const axgl::gui::Page::Context& context) = 0;
  virtual void on_blur(const axgl::gui::Page::Context& context) = 0;
};

} // namespace axgl::gui
