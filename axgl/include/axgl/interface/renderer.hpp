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
  virtual void set_window(axgl::ptr_t<axgl::Window> window) = 0;
  virtual void render(const axgl::Service::Context& context, axgl::ptr_t<axgl::Realm> realm) = 0;
};

} // namespace axgl
