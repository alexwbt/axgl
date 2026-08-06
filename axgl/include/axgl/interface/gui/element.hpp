#pragma once

#include <axgl/interface/gui/context.hpp>
#include <axgl/interface/gui/page.hpp>
#include <axgl/interface/gui/style.hpp>

namespace axgl::gui
{

class Element
{
public:
  virtual ~Element() = default;

  [[nodiscard]] virtual std::uint64_t get_id() const = 0;
  [[nodiscard]] virtual glm::vec2 get_position() const = 0;
  [[nodiscard]] virtual glm::vec2 get_size() const = 0;
  [[nodiscard]] virtual glm::vec4 get_rect() const = 0;
  [[nodiscard]] virtual glm::vec4 get_visible_rect() const = 0;
  [[nodiscard]] virtual bool is_focusable() const = 0;
  [[nodiscard]] virtual bool is_focused() const = 0;
  [[nodiscard]] virtual bool is_hovering() const = 0;
  [[nodiscard]] virtual bool is_activated() const = 0;

  [[nodiscard]] virtual axgl::gui::Style* style() const = 0;
  [[nodiscard]] virtual axgl::Container<axgl::gui::Element>& children() = 0;

  virtual void init(const axgl::gui::Context& context) = 0;
  virtual void update(const axgl::gui::Context& context) = 0;
  virtual void render(const axgl::gui::Context& context) = 0;

  virtual void on_pointer_enter(const axgl::gui::Context& context) = 0;
  virtual void on_pointer_exit(const axgl::gui::Context& context) = 0;
  virtual void on_activate(const axgl::gui::Context& context) = 0;
  virtual void on_deactivate(const axgl::gui::Context& context) = 0;
  virtual void on_focus(const axgl::gui::Context& context) = 0;
  virtual void on_blur(const axgl::gui::Context& context) = 0;

  virtual void set_position(glm::vec2 position) = 0;
  virtual void set_size(glm::vec2 size) = 0;

  virtual axgl::gui::Style* set_style(const std::vector<std::string>& styles)
    = 0;
  virtual void append_style(const std::string& style) = 0;
  virtual void remove_style(const std::string& style) = 0;
};

} // namespace axgl::gui
