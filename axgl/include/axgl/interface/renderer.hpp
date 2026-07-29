#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/context.hpp>

namespace axgl
{

class Realm;
class Camera;
class Window;
class Light;

class Renderer : virtual public axgl::ContextHolder
{
public:
  enum class Flag
  {
    kMultiSample,
    kShadow,
  };
  enum class ValueKey
  {
    kMultiSampleCount,
    kShadowMapSize,
  };

  virtual void set_window(axgl::ptr_t<axgl::Window> window) = 0;
  [[nodiscard]] virtual axgl::ptr_t<axgl::Window> get_window() const = 0;

  virtual void set_flag(Flag flag, bool enable) = 0;
  virtual void set_value(ValueKey key, const std::string& value) = 0;
  [[nodiscard]] virtual bool get_flag(Flag flag) const = 0;
  [[nodiscard]] virtual std::string get_value(ValueKey key) const = 0;

  void toggle_flag(Flag flag) { set_flag(flag, !get_flag(flag)); }

  template <typename T>
  void set_value(ValueKey key, T value)
  {
    set_value(key, std::to_string(value));
  }

  virtual void render() = 0;
};

} // namespace axgl
