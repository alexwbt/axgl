#pragma once

#include <cstdint>
#include <memory>

#include <axgl/interface/input.hpp>
#include <axgl/interface/pointer.hpp>
#include <axgl/interface/services/window_service.hpp>

namespace axgl
{

class InputService : virtual public Service
{
public:
  enum class CursorMode
  {
    kLocked,
    kNormal,
  };

  virtual void set_window(std::shared_ptr<Window> window) = 0;
  virtual void add_input(std::shared_ptr<Input> input) = 0;
  virtual void add_pointer(std::shared_ptr<Pointer> pointer) = 0;
  virtual void remove_input(std::uint32_t id) = 0;
  virtual void remove_pointer(std::uint32_t id) = 0;

  virtual void set_cursor_mode(CursorMode mode) = 0;

  friend class Input;
  friend class Pointer;
};

} // namespace axgl

