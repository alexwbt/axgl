#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/bullet/physics_service.hpp>
#include <axgl/impl/bullet/rigidbody.hpp>
#include <axgl/impl/bullet/shapes.hpp>

namespace axgl {

inline void configure_bullet(Axgl& axgl) {
  axgl.register_service_t<impl::bullet::PhysicsService>();
  axgl.entity_service()
    ->register_component_t<impl::bullet::component::RigidBody>();
  axgl.physics_service()->register_shape_t<impl::bullet::BoxShape>();
  axgl.physics_service()->register_shape_t<impl::bullet::SphereShape>();
}

} // namespace axgl
