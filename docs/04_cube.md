# 4. Cube

A 3D scene adds a perspective camera, a light, and 3D meshes. Scene construction
can live in a custom `Service`, which is the usual way to package an
application's setup and per-frame behavior.

## Services

`Service` (in `axgl/interface/service.hpp`) defines lifecycle hooks:

```cpp
class Service {
public:
  virtual void initialize() {}
  virtual void terminate() {}
  virtual void on_start() {}
  virtual void on_end() {}
  virtual void tick() {}
  virtual void update() {}
  virtual void render() {}
  virtual bool running() { return true; }
  virtual bool keep_alive() { return false; }
  virtual void exec(const std::vector<std::string>&) {}
  [[nodiscard]] virtual std::int32_t priority() const { return 0; }
};
```

A custom service registers before `initialize()`:

```cpp
class Application final : public axgl::Service {
public:
  static constexpr std::string_view kTypeId = "app";

  void on_start() override { /* build the scene */ }
  void tick() override { /* advance simulation */ }
};

axgl.register_service_t<Application>();
axgl.initialize();
```

`on_start` runs once before the loop; `tick`, `update`, and `render` run each
frame. Services are ordered by `priority()`.

## Perspective camera

A camera entity placed at `z = -2` looking toward the origin gives a standard 3D
view:

```cpp
const auto camera_entity = axgl.entity_service()->create_entity();
const auto camera = axgl.entity_service()->create_component_t<CameraComponent>();
camera_entity->add_component(camera);
camera_entity->transform().position.z = -2;
camera_entity->transform().update_matrix();
axgl.camera_service()->set_camera_entity(camera_entity);
```

The `axgl::Camera` defaults are perspective (`orthographic = false`) with a
field of view, near/far clips, and a viewport.

## 3D meshes and lights

```cpp
const auto material = axgl.renderer_service()->create_material("3d");
material->set_color({1.0f, 0.5f, 0.2f, 1.0f});

const auto mesh = axgl.entity_service()->create_component_t<axgl::component::Mesh>();
axgl::mesh::init_cube(*mesh);
mesh->set_material(material);

const auto light_entity = axgl.entity_service()->create_entity();
const auto light = axgl.entity_service()->create_component_t<LightComponent>();
light->light = axgl::Light::sunlight({0.2f, -1.0f, 1.2f});
light_entity->add_component(light);
```

`axgl::Light` provides `sunlight`, `point_light`, and `spotlight` factories.
`axgl::mesh::init_quad`, `init_plain`, and `init_cube` populate mesh components
with standard geometry.

## Transform

Every entity has a `Transform` with `position`, `rotation`, `scale`, and
`pivot`. After changing any of them, call `update_matrix()` (the `set_position`
/ `set_rotation` / `set_scale` helpers do this for you):

```cpp
auto& transform = cube_entity->transform();
transform.rotation += glm::vec3(0.01f, 0.02f, 0.05f);
transform.update_matrix();
```

Example: `demo/04_cube`.
