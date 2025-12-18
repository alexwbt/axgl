#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/input.hpp>
#include <axgl/interface/pointer.hpp>
#include <axgl/interface/services/window_service.hpp>

namespace axgl
{

class InputService : virtual public axgl::Service
{
public:
  static constexpr std::string_view kTypeId = "service::input";

  enum class CursorMode
  {
    kLocked,
    kNormal,
  };

  virtual void set_window(axgl::ptr_t<axgl::Window> window) = 0;
  virtual void add_input(axgl::ptr_t<axgl::Input> input) = 0;
  virtual void add_pointer(axgl::ptr_t<axgl::Pointer> pointer) = 0;
  virtual void remove_input(std::uint32_t id) = 0;
  virtual void remove_pointer(std::uint32_t id) = 0;

  virtual void set_cursor_mode(axgl::InputService::CursorMode mode) = 0;

  friend class axgl::Input;
  friend class axgl::Pointer;
};

} // namespace axgl

