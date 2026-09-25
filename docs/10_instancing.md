# 10. Instancing

Entity types can be extended with their own lifecycle behavior, and the engine
is built to hold large numbers of them. This topic covers subclassing `Entity`,
registering the type, frame-rate-independent updates, and the static hint used
for batching.

## Entity behavior

`Entity` defines lifecycle hooks that run with the entity's tree:

```cpp
class Entity {
public:
  virtual void tick() {}
  virtual void update() {}
  virtual void on_create() {}
  virtual void on_remove() {}
  // transform(), children(), components(), ...
};
```

A custom type declares a `kTypeId` and overrides those hooks:

```cpp
class Box final : public axgl::Entity {
public:
  static constexpr std::string_view kTypeId = "entity::box";

  void on_create() override { /* capture initial state */ }

  void update() override {
    auto& transform = this->transform();
    theta_ += orbit_speed_ * context_->delta_tick_f / radius_;
    transform.position.x = std::cos(theta_) * radius_;
    transform.position.z = std::sin(theta_) * radius_;
    transform.rotation += rotation_speed_ * context_->delta_tick_f;
    transform.update_matrix();
  }
};
```

`context_->delta_tick_f` is the seconds elapsed since the last step, so motion
scales with elapsed time rather than frame count.

## Registration and creation

```cpp
axgl.entity_service()->register_entity_t<Box>();
const auto box = axgl.entity_service()->create_entity_t<Box>();
```

`register_entity_t` / `register_component_t` register factories by type id;
`create_entity_t` / `create_component_t` instantiate and cast. The same pattern
applies to components, realms, and services.

## Scaling to many entities

Entities can be generated in bulk and added to the realm's container:

```cpp
for (int i = 0; i < count; ++i) {
  auto entity = create_entity(axgl);
  entity->transform().position = /* scattered */;
  entity->transform().update_matrix();
  realm->entities().add(entity);
}
```

The transform is part of the scene graph: moving a parent moves its children,
which is what makes per-entity animation cheap to express.

An entity can be marked static to signal that it does not move and can be
batched by the renderer:

```cpp
grass->set_static(true);
```

The sharing pattern in `axgl::mesh` helpers and a single material per kind of
entity keeps the number of GPU resources low even when the entity count is
large.

Example: `demo/10_instancing`.

