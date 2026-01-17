#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/container.hpp>
#include <axgl/interface/service.hpp>

namespace axgl
{

class Entity;

class Realm
{
public:
  struct Context : axgl::Service::Context
  {
    axgl::Realm& realm;
  };

  virtual ~Realm() = default;
  virtual void initialize(const axgl::Service::Context& context) = 0;
  virtual void update(const axgl::Service::Context& context) = 0;
  virtual void tick(const axgl::Service::Context& context) = 0;
  [[nodiscard]] virtual axgl::Container<axgl::Entity>& entities() = 0;
};

} // namespace axgl
