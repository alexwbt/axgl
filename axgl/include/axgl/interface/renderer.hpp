#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/service.hpp>

namespace axgl
{

class Realm;
class Camera;
class Window;
class Light;

class Renderer
{
public:
  virtual ~Renderer() = default;
  virtual void set_antialiasing(bool enable) = 0;
  virtual void set_sample_count(int sample_count) = 0;
  virtual void set_window(axgl::ptr_t<axgl::Window> window) = 0;
  [[nodiscard]] virtual bool get_antialiasing() const = 0;
  [[nodiscard]] virtual int get_sample_count() const = 0;
  [[nodiscard]] virtual axgl::ptr_t<axgl::Window> get_window() const = 0;

  virtual void render(const axgl::Service::Context& context) = 0;
};

} // namespace axgl
