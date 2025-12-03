#pragma once

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
  virtual void before_render() = 0;
  virtual void after_render() = 0;

  virtual void set_window(axgl::ptr_t<axgl::Window> window) = 0;
  [[nodiscard]] virtual glm::ivec2 viewport() const = 0;

  // virtual void render(axgl::ptr_t<axgl::Realm> realm, axgl::ptr_t<axgl::Texture> target) = 0;
};

} // namespace axgl
