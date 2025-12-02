#pragma once

#include <span>

#include <axgl/common.hpp>
#include <axgl/interface/light.hpp>
#include <axgl/interface/window.hpp>

namespace axgl
{

class Renderer
{
public:
  virtual ~Renderer() = default;
  virtual bool ready() = 0;
  virtual void render() = 0;
  virtual void set_window(axgl::ptr_t<axgl::Window> window) = 0;
  [[nodiscard]] virtual glm::ivec2 viewport() const = 0;

  virtual void add_light(const axgl::Light* light) = 0;
  [[nodiscard]] virtual std::span<const axgl::Light* const> lights() const = 0;
};

} // namespace axgl
