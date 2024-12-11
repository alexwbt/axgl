
#define AXGL_DEBUG
#include <axgl/axgl.hpp>
#include <axgl/impl/realm_service.hpp>
#include <axgl/impl/glfw/window.hpp>
#include <axgl/impl/opengl/renderer.hpp>

int main()
{
  axgl::Axgl axgl;
  axgl.use_service<axgl::impl::GlfwWindowService>();
  axgl.use_service<axgl::impl::OpenglRendererService>();
  axgl.use_service<axgl::impl::RealmService>();

  // window
  auto window = axgl.window_service()->create_window();
  window->set_title("Hello triangle!");

  // renderer
  auto renderer_service = axgl.renderer_service();
  auto renderer = renderer_service->create_renderer();
  renderer->set_window(window);

  // realm
  auto realm_service = axgl.realm_service();
  auto realm = realm_service->create_realm();
  realm->set_renderer(renderer);
  realm->camera.orthographic = true;
  realm->camera.near_clip = -1;
  realm->camera.far_clip = 1;

  // triangle mesh
  auto mesh = realm_service->create_component<axgl::interface::Mesh>();
  mesh->set_vertices(std::vector<glm::vec2>{ {0.8f, -0.5f}, { -0.8f, -0.5f }, { 0.0f, 0.5f } });
  mesh->scale = glm::vec3(200.0f);

  // material
  auto material = renderer_service->create_material("2d");
  material->set_color({ 1.0f, 0.5f, 0.2f });
  mesh->set_material(material);

  // triangle entity
  auto entity = realm->create_entity();
  entity->add_component(mesh);

  // start
  axgl.run();
}
