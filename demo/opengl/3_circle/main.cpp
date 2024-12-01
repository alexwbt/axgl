
#include <numbers>

#define AXGL_DEBUG
#include <axgl/axgl.hpp>
#include <axgl/impl/realm_service.hpp>
#include <axgl/impl/glfw/glfw_service.hpp>
#include <axgl/impl/opengl/opengl_service.hpp>

void circle_mesh(std::shared_ptr<axgl::interface::Mesh2D> mesh, uint32_t vert_count)
{
  std::vector<glm::vec2> vertices;
  std::vector<uint32_t> indices;

  vertices.reserve(vert_count + 1);
  indices.reserve(vert_count * 3);

  float delta = (2 * std::numbers::pi) / vert_count;
  for (int i = 0; i < vert_count; ++i)
  {
    auto r = i * delta;
    vertices.push_back({ sin(r), cos(r) });
    indices.push_back(i);
    indices.push_back((i + 1) % vert_count);
    indices.push_back(vert_count);
  }
  vertices.push_back({ 0, 0 });

  mesh->set_indices(indices);
  mesh->set_vertices(vertices);
  mesh->set_color({ 1.0f, 0.5f, 0.2f });
  mesh->scale = glm::vec3(200.0f);
}

int main()
{
  axgl::Axgl axgl;
  axgl.use_service<axgl::impl::GlfwWindowService>();
  axgl.use_service<axgl::impl::OpenglRendererService>();
  axgl.use_service<axgl::impl::DefaultRealmService>();

  // window
  auto window = axgl.window_service()->create_window();
  window->set_title("Hello circle!");

  // renderer
  auto renderer = axgl.renderer_service()->create_renderer();
  renderer->set_window(window);

  // realm
  auto realm = axgl.realm_service()->create_realm();
  realm->set_renderer(renderer);
  realm->camera.orthographic = true;
  realm->camera.near_clip = -1;
  realm->camera.far_clip = 1;

  // circle mesh
  auto mesh = axgl.create_component<axgl::interface::Mesh2D>();
  circle_mesh(mesh, 50);

  // circle entity
  auto entity = realm->create_entity();
  entity->add_component(mesh);

  // start
  axgl.run();
}
