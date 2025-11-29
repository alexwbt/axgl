#pragma once

#include <memory>

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
  struct Context : Service::Context
  {
    Realm& realm;
  };

  virtual ~Realm() = default;
  virtual void tick(const Service::Context& context) = 0;
  virtual void update(const Service::Context& context) = 0;
  virtual void render(const Service::Context& context) = 0;

  virtual void set_renderer(std::shared_ptr<Renderer> renderer) = 0;
  [[nodiscard]] virtual std::shared_ptr<Renderer> get_renderer() const = 0;

  virtual void add_entity(std::shared_ptr<Entity> entity) = 0;
  virtual void remove_entity(std::shared_ptr<Entity> entity) = 0;
  [[nodiscard]] virtual util::Iterable<std::shared_ptr<Entity>> get_entities() const = 0;
};

} // namespace axgl
