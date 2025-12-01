#pragma once

#include <memory>

#include <axgl/common.hpp>
#include <axgl/interface/renderer.hpp>
#include <axgl/interface/service.hpp>

#include <axgl/util/iterable.hpp>

namespace axgl
{

class Axgl;
class Entity;

class Realm
{
public:
  struct Context : axgl::Service::Context
  {
    Realm& realm;
    Renderer* renderer;
  };

  virtual ~Realm() = default;
  virtual void tick(const axgl::Service::Context& context) = 0;
  virtual void update(const axgl::Service::Context& context) = 0;
  virtual void render(const axgl::Service::Context& context) = 0;

  virtual void set_renderer(axgl::ptr_t<axgl::Renderer> renderer) = 0;
  [[nodiscard]] virtual axgl::ptr_t<axgl::Renderer> get_renderer() const = 0;

  [[nodiscard]] virtual axgl::Container<axgl::Entity>& entities() = 0;
};

} // namespace axgl
