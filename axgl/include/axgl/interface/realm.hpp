#pragma once

#include <axgl/interface/container.hpp>
#include <axgl/interface/context.hpp>

namespace axgl
{

class Entity;

class Realm : virtual public axgl::ContextHolder
{
public:
  virtual void initialize() = 0;
  virtual void update() = 0;
  virtual void tick() = 0;
  [[nodiscard]] virtual axgl::Container<axgl::Entity>& entities() = 0;
};

} // namespace axgl
