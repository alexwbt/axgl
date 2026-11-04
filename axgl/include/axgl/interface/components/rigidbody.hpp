#pragma once

#include <glm/glm.hpp>

#include <axgl/common.hpp>
#include <axgl/interface/component.hpp>
#include <axgl/interface/physics_shape.hpp>

namespace axgl::component {

class RigidBody : public virtual axgl::Component {
public:
  static constexpr std::string_view kTypeId = "component::rigidbody";

  virtual void set_mass(float mass) = 0;
  [[nodiscard]] virtual float get_mass() const = 0;

  virtual void set_shape(axgl::ptr_t<axgl::PhysicsShape> shape) = 0;
  [[nodiscard]] virtual axgl::ptr_t<axgl::PhysicsShape> get_shape() const = 0;

  virtual void set_velocity(const glm::vec3& velocity) = 0;
  [[nodiscard]] virtual glm::vec3 get_velocity() const = 0;

  virtual void apply_force(const glm::vec3& force) = 0;
  virtual void apply_impulse(const glm::vec3& impulse) = 0;

  virtual void set_friction(float friction) = 0;
  [[nodiscard]] virtual float get_friction() const = 0;

  virtual void set_restitution(float restitution) = 0;
  [[nodiscard]] virtual float get_restitution() const = 0;
};

} // namespace axgl::component
