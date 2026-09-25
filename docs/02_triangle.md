# 2. Triangle

2D rendering is done with ordinary entities, meshes, and materials, viewed
through an orthographic camera. There is no separate 2D pipeline; the difference
from 3D is the camera projection and the shape of the vertex data.

## Renderer and realm

Rendering needs a `Renderer` attached to a window, and a `Realm` to hold the
entities:

```cpp
const auto renderer = axgl.renderer_service()->create_renderer();
renderer->set_window(window);
renderer->set_enable_msaa(true);
renderer->set_msaa_sample_count(8);
axgl.renderer_service()->set_active_renderer(renderer);

const auto realm = axgl.realm_service()->create_realm();
axgl.realm_service()->set_active_realm(realm);
```

## Meshes

A mesh is a `component::Mesh`:

```cpp
const auto material = axgl.renderer_service()->create_material("2d");
material->set_color({1.0f, 0.5f, 0.2f, 1.0f});

const auto mesh = axgl.entity_service()->create_component_t<axgl::component::Mesh>();
mesh->set_vertices(std::vector<glm::vec2>{
  {0.8f, -0.5f}, {-0.8f, -0.5f}, {0.0f, 0.5f}
});
mesh->set_material(material);
entity->add_component(mesh);
```

`component::Mesh` accepts 2D (`glm::vec2`) or 3D (`glm::vec3`) vertices and
optional normals, tangents, bitangents, UVs, and indices. Material objects are
created by type; `"2d"` selects the 2D shading path.

## Orthographic camera

```cpp
const auto camera = axgl.entity_service()->create_component_t<CameraComponent>();
camera->camera.orthographic = true;
camera->camera.near_clip = -1;
camera->camera.far_clip = 1;
entity->add_component(camera);
entity->set_scale(glm::vec3(200.0f));

realm->entities().add(entity);
axgl.camera_service()->set_camera_entity(entity);
```

The camera component wraps an `axgl::Camera`. `orthographic` with near/far clips
of -1/1 maps the unit quad to the screen, and the entity's scale converts that
to pixels. A single entity may carry both the mesh and the camera.

`set_camera_entity` selects which entity's camera the renderer uses.

Example: `demo/02_triangle`.
