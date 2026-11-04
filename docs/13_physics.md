# 13. Physics

Physics adds a world simulation that moves entities from frame to frame. The `axgl`
interfaces expose a physics service and a rigid-body component; a concrete backend
(such as the Bullet one) implements them.

## PhysicsService

`PhysicsService` (in `axgl/interface/services/physics_service.hpp`) owns the
simulation world:

```cpp
class PhysicsService {
public:
  virtual void set_gravity(const glm::vec3& gravity) = 0;
  [[nodiscard]] virtual glm::vec3 get_gravity() const = 0;
  virtual void step_simulation(float time_step) = 0;

  virtual void register_shape_factory(
    const std::string& shape_id,
    std::function<axgl::ptr_t<axgl::PhysicsShape>()> shape_factory
  ) = 0;
  virtual axgl::ptr_t<axgl::PhysicsShape> create_shape(
    const std::string& shape_id
  ) = 0;
};
```

The `register_shape_t` / `create_shape_t` helpers register and instantiate a given
shape type by its `kTypeId`, mirroring the entity/component factories.

```cpp
const auto& physics = axgl.physics_service();
physics->set_gravity(glm::vec3(0.0f, -9.8f, 0.0f));
```

## RigidBody component

`RigidBody` (in `axgl/interface/components/rigidbody.hpp`) attaches physics
behavior to an entity:

```cpp
class RigidBody {
public:
  virtual void set_mass(float mass) = 0;
  virtual void set_velocity(const glm::vec3& velocity) = 0;
  virtual void apply_force(const glm::vec3& force) = 0;
  virtual void apply_impulse(const glm::vec3& impulse) = 0;
  virtual void set_friction(float friction) = 0;
  virtual void set_restitution(float restitution) = 0;
  virtual void set_shape(axgl::ptr_t<axgl::PhysicsShape> shape) = 0;
};
```

A body with `mass = 0` is static (immovable); a positive mass makes it dynamic.
Each frame the component keeps the entity's `Transform` in sync with the simulated
body.

## PhysicsShape

`PhysicsShape` (in `axgl/interface/physics_shape.hpp`) is the base for collision
shapes:

```cpp
class PhysicsShape {
public:
  virtual ~PhysicsShape() = default;
};
```

Concrete shapes (box, sphere, and so on) are defined by the backend and created
through the physics service.

## Usage

```cpp
const auto box = entity_service->create_entity();
box->transform().position.y = 6.0f;

const auto shape = physics->create_shape_t<BoxShape>();
shape->set_half_extents({0.5f, 0.5f, 0.5f});

const auto body = entity_service->create_component_t<RigidBody>();
body->set_shape(shape);
body->set_mass(1.0f);
box->add_component(body);
```

Example: `demo/13_physics`.
