#pragma once

#include <memory>

#include <btBulletDynamicsCommon.h>

#include <axgl/impl/bullet/physics_shape.hpp>

namespace axgl::impl::bullet {

class BoxShape : public axgl::impl::bullet::PhysicsShape {
public:
  static constexpr std::string_view kTypeId = "physics_shape::box";

  void set_half_extents(const glm::vec3& half_extents) {
    shape = std::make_unique<btBoxShape>(
      btVector3(half_extents.x, half_extents.y, half_extents.z)
    );
  }
};

class SphereShape : public axgl::impl::bullet::PhysicsShape {
public:
  static constexpr std::string_view kTypeId = "physics_shape::sphere";

  void set_radius(float radius) {
    shape = std::make_unique<btSphereShape>(radius);
  }
};

} // namespace axgl::impl::bullet
