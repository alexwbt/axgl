#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/container.hpp>
#include <axgl/interface/service.hpp>

namespace axgl
{

class Axgl;
class Light;
class Camera;
class Entity;
class Renderer;

class Realm
{
public:
  struct Context : axgl::Service::Context
  {
    axgl::Realm& realm;
    axgl::ptr_t<axgl::Renderer> renderer;
    const axgl::Camera* camera;
    std::vector<const axgl::Light*>& lights;
  };

  virtual ~Realm() = default;
  virtual void tick(const axgl::Service::Context& context) { }
  virtual void update(const axgl::Service::Context& context) { }
  virtual void render(const axgl::Service::Context& context) { }

  virtual void set_renderer(axgl::ptr_t<axgl::Renderer> renderer) = 0;
  [[nodiscard]] virtual axgl::ptr_t<axgl::Renderer> get_renderer() const = 0;

  [[nodiscard]] virtual axgl::Container<axgl::Entity>& entities() = 0;
};

} // namespace axgl
