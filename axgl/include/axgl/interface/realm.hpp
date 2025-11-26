#pragma once

#include <memory>
#include <vector>

#include <axgl/interface/camera.hpp>
#include <axgl/interface/light.hpp>
#include <axgl/interface/renderer.hpp>

#include <axgl/util/iterable.hpp>

namespace axgl
{

class Axgl;
class Entity;

class Realm
{
public:
  struct Context final
  {
    const Axgl* axgl = nullptr;
    Realm* realm = nullptr;
  };

  virtual ~Realm() = default;
  virtual void set_context(Context* context) = 0;
  virtual void set_renderer(std::shared_ptr<Renderer> renderer) = 0;
  [[nodiscard]] virtual std::shared_ptr<Renderer> get_renderer() const = 0;

  virtual void tick() = 0;
  virtual void update() = 0;
  virtual void render() = 0;

  virtual void add_entity(std::shared_ptr<Entity> entity) = 0;
  virtual void remove_entity(std::shared_ptr<Entity> entity) = 0;
  [[nodiscard]] virtual util::Iterable<std::shared_ptr<Entity>> get_entities() const = 0;
};

} // namespace axgl
