#include <numbers>

#include <axgl/axgl.hpp>
#include <axgl/impl/glfw.hpp>
#include <axgl/impl/opengl.hpp>
#include <axgl/impl/realm_service.hpp>
#include <axgl/impl/component/camera.hpp>

static void circle_mesh(const std::shared_ptr<axgl::interface::component::Mesh>& mesh, const size_t vert_count)
{
  std::vector<glm::vec2> vertices;
  std::vector<uint32_t> indices;

  vertices.reserve(vert_count + 1);
  indices.reserve(vert_count * 3);

  const float delta = static_cast<float>(2 * std::numbers::pi) / static_cast<float>(vert_count);
  for (int i = 0; i < vert_count; ++i)
  {
    const auto r = static_cast<float>(i) * delta;
    vertices.emplace_back(sin(r), cos(r));
    indices.push_back(i);
    indices.push_back((i + 1) % vert_count);
    indices.push_back(vert_count);
  }
  vertices.emplace_back(0, 0);

  mesh->set_indices(indices);
  mesh->set_vertices(vertices);
}

int main()
{
  axgl::Axgl axgl;
  axgl.use_service<axgl::impl::GlfwWindowService>();
  axgl.use_service<axgl::impl::OpenglRendererService>();
  axgl.use_service<axgl::impl::RealmService>();

  // window
  const auto window = axgl.window_service()->create_window();
  window->set_title("Hello circle!");

  // renderer
  const auto renderer_service = axgl.renderer_service();
  const auto renderer = renderer_service->create_renderer();
  renderer->set_window(window);

  // realm
  const auto realm_service = axgl.realm_service();
  const auto realm = realm_service->create_realm();
  realm->set_renderer(renderer);

  // circle entity
  const auto entity = realm_service->create_entity<axgl::interface::Entity>(); {
    // material
    const auto material = renderer_service->create_material("2d");
    material->set_color({ 1.0f, 0.5f, 0.2f, 1.0f });

    // circle mesh
    const auto mesh_comp = realm_service->create_component<axgl::interface::component::Mesh>();
    circle_mesh(mesh_comp, 50);
    mesh_comp->set_material(material);
    entity->add_component(mesh_comp);

    // camera
    const auto camera_comp = realm_service->create_component<axgl::impl::component::Camera>();
    camera_comp->camera.orthographic = true;
    camera_comp->camera.near_clip = -1;
    camera_comp->camera.far_clip = 1;
    entity->add_component(camera_comp);
  }
  entity->transform()->scale = glm::vec3(200.0f);
  entity->update_model_matrix();
  realm->add_entity(entity);

  // start
  axgl.run();
}
