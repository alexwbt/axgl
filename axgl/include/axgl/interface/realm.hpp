#pragma once

#include <axgl/interface/container.hpp>

namespace axgl
{

class Entity;

class Realm
{
public:
  virtual ~Realm() = default;
  virtual void initialize() = 0;
  virtual void update() = 0;
  virtual void tick() = 0;
  [[nodiscard]] virtual axgl::Container<axgl::Entity>& entities() = 0;
};

} // namespace axgl
