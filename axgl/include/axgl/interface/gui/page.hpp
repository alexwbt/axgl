#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/container.hpp>
#include <axgl/interface/context.hpp>
#include <axgl/interface/gui/style.hpp>
#include <axgl/interface/input.hpp>
#include <axgl/interface/pointer.hpp>

namespace axgl
{

class Texture;

} // namespace axgl

namespace axgl::gui
{

class Element;

class Page : virtual public axgl::ContextHolder
{
public:
  virtual void set_size(std::uint32_t width, std::uint32_t height) = 0;
  virtual void set_scale(float scale) = 0;
  virtual void set_should_render(bool should_render) = 0;
  virtual void init() = 0;
  virtual void update() = 0;
  virtual void render() = 0;
  [[nodiscard]] virtual bool should_render() const = 0;
  [[nodiscard]] virtual glm::ivec2 get_size() const = 0;
  [[nodiscard]] virtual axgl::ptr_t<axgl::Texture> get_texture() const = 0;
  [[nodiscard]] virtual axgl::Container<axgl::gui::Element>& elements() = 0;

  virtual void set_cursor_type(axgl::gui::CursorType cursor_type) = 0;
  [[nodiscard]] virtual axgl::gui::CursorType get_cursor_type() = 0;

  virtual void set_cursor_pointer(axgl::ptr_t<axgl::Pointer> cursor) = 0;
  virtual void set_scroll_pointer(axgl::ptr_t<axgl::Pointer> scroll) = 0;
  virtual void set_scale_input(axgl::ptr_t<axgl::Input> scale) = 0;
  virtual void set_activate_input(axgl::ptr_t<axgl::Input> activate) = 0;
  virtual void set_focus_switch_input(axgl::ptr_t<axgl::Input> focus_switch) = 0;
  virtual void set_focus_activate_input(axgl::ptr_t<axgl::Input> focus_activate) = 0;
  [[nodiscard]] virtual axgl::ptr_t<axgl::Pointer> get_cursor_pointer() const = 0;
  [[nodiscard]] virtual axgl::ptr_t<axgl::Pointer> get_scroll_pointer() const = 0;
  [[nodiscard]] virtual axgl::ptr_t<axgl::Input> get_scale_input() const = 0;
  [[nodiscard]] virtual axgl::ptr_t<axgl::Input> get_activate_input() const = 0;
  [[nodiscard]] virtual axgl::ptr_t<axgl::Input> get_focus_switch_input() const = 0;
  [[nodiscard]] virtual axgl::ptr_t<axgl::Input> get_focus_activate_input() const = 0;
};

} // namespace axgl::gui
