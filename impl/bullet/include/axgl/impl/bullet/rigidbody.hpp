#pragma once

#include <memory>

#include <btBulletDynamicsCommon.h>

#include <axgl/common.hpp>
#include <axgl/interface/components/rigidbody.hpp>
#include <axgl/interface/services/physics_service.hpp>

#include <axgl/impl/bullet/physics_service.hpp>
#include <axgl/impl/bullet/physics_shape.hpp>
#include <axgl/impl/component_base.hpp>

namespace axgl::impl::bullet::component {

class RigidBody : public virtual axgl::component::RigidBody,
                  public axgl::impl::ComponentBase {
public:
  static constexpr std::string_view kTypeId = "component::rigidbody";

private:
  float mass_ = 0.0f;
  float friction_ = 0.5f;
  float restitution_ = 0.0f;
  axgl::ptr_t<axgl::PhysicsShape> shape_;
  std::unique_ptr<btRigidBody> body_;
  glm::vec3 velocity_{0.0f};

  static void sync_bullet_to_transform(
    const btRigidBody& body, axgl::Transform& transform
  ) {
    const auto& origin = body.getCenterOfMassPosition();
    const btQuaternion rotation = body.getOrientation();
    transform.position = glm::vec3(origin.x(), origin.y(), origin.z());
    transform.rotation = glm::eulerAngles(
      glm::quat(rotation.w(), rotation.x(), rotation.y(), rotation.z())
    );
    transform.update_matrix();
  }

  void rebuild() {
    body_.reset();
    auto* bt_shape = shape_
      ? static_cast<axgl::impl::bullet::PhysicsShape*>(shape_.get())
          ->shape.get()
      : nullptr;
    if (!bt_shape) return;
    btVector3 local_inertia(0, 0, 0);
    if (mass_ > 0.0f) bt_shape->calculateLocalInertia(mass_, local_inertia);
    btRigidBody::btRigidBodyConstructionInfo info(
      mass_, nullptr, bt_shape, local_inertia
    );
    info.m_friction = friction_;
    info.m_restitution = restitution_;
    body_ = std::make_unique<btRigidBody>(info);
    body_->setLinearVelocity(btVector3(velocity_.x, velocity_.y, velocity_.z));
  }

  void add_to_world() {
    if (!body_) return;
    if (
      auto service = axgl::ptr_cast<axgl::impl::bullet::PhysicsService>(
        axgl_->physics_service()
      )
    ) {
      service->world()->addRigidBody(body_.get());
    }
  }

  void remove_from_world() {
    if (!body_) return;
    if (
      auto service = axgl::ptr_cast<axgl::impl::bullet::PhysicsService>(
        axgl_->physics_service()
      )
    ) {
      service->world()->removeRigidBody(body_.get());
    }
  }

public:
  void on_parent_create(axgl::Entity* entity) override {
    ComponentBase::on_parent_create(entity);
    if (body_) {
      btTransform transform;
      transform.setIdentity();
      transform.setOrigin(btVector3(
        entity->transform().position.x,
        entity->transform().position.y,
        entity->transform().position.z
      ));
      body_->setWorldTransform(transform);
    }
    add_to_world();
  }

  void on_parent_remove(axgl::Entity* entity) override {
    remove_from_world();
    ComponentBase::on_parent_remove(entity);
  }

  void parent_update(axgl::Entity* entity) override {
    if (!body_) return;
    sync_bullet_to_transform(*body_, entity->transform());
  }

  void set_mass(float mass) override {
    mass_ = mass;
    remove_from_world();
    rebuild();
    add_to_world();
  }

  [[nodiscard]] float get_mass() const override { return mass_; }

  void set_shape(axgl::ptr_t<axgl::PhysicsShape> shape) override {
    shape_ = std::move(shape);
    remove_from_world();
    rebuild();
    add_to_world();
  }

  [[nodiscard]] axgl::ptr_t<axgl::PhysicsShape> get_shape() const override {
    return shape_;
  }

  void set_velocity(const glm::vec3& velocity) override {
    velocity_ = velocity;
    if (body_)
      body_->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
  }

  [[nodiscard]] glm::vec3 get_velocity() const override {
    if (body_) {
      const auto v = body_->getLinearVelocity();
      return glm::vec3(v.x(), v.y(), v.z());
    }
    return velocity_;
  }

  void apply_force(const glm::vec3& force) override {
    if (body_) body_->applyCentralForce(btVector3(force.x, force.y, force.z));
  }

  void apply_impulse(const glm::vec3& impulse) override {
    if (body_)
      body_->applyCentralImpulse(btVector3(impulse.x, impulse.y, impulse.z));
  }

  void set_friction(float friction) override {
    friction_ = friction;
    if (body_) body_->setFriction(friction);
  }

  [[nodiscard]] float get_friction() const override { return friction_; }

  void set_restitution(float restitution) override {
    restitution_ = restitution;
    if (body_) body_->setRestitution(restitution);
  }

  [[nodiscard]] float get_restitution() const override { return restitution_; }
};

} // namespace axgl::impl::bullet::component
