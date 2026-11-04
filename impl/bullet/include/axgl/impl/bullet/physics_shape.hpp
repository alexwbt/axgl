#pragma once

#include <memory>

#include <btBulletDynamicsCommon.h>

#include <axgl/interface/physics_shape.hpp>

#include <axgl/impl/context_holder.hpp>

namespace axgl::impl::bullet {

class PhysicsShape : public axgl::PhysicsShape,
                     public axgl::impl::ContextHolder {
public:
  std::unique_ptr<btCollisionShape> shape;
};

} // namespace axgl::impl::bullet
