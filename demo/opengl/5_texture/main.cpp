
#include <vector>

#define AXGL_DEBUG
#include <axgl/axgl.hpp>
#include <axgl/impl/realm_service.hpp>
#include <axgl/impl/glfw/glfw_service.hpp>
#include <axgl/impl/opengl/opengl_service.hpp>

#include "demo_opengl_texture/res.hpp"

int main()
{
  axgl::Axgl axgl;
  axgl.use_service<axgl::impl::GlfwWindowService>();
  axgl.use_service<axgl::impl::OpenglRendererService>();
  axgl.use_service<axgl::impl::DefaultRealmService>();

  // window
  auto window = axgl.window_service()->create_window();
  window->set_title("Hello texture!");

  // renderer
  auto renderer = axgl.renderer_service()->create_renderer();
  renderer->set_window(window);

  // realm
  auto realm = axgl.realm_service()->create_realm();
  realm->set_renderer(renderer);
  realm->camera.orthographic = true;
  realm->camera.near_clip = -1;
  realm->camera.far_clip = 1;

  // texture
  auto texture = renderer->create_texture();
  texture->load_2d_texture(demo_opengl_texture_res::get("container.png"));

  // square mesh
  auto mesh = axgl.create_component<axgl::interface::Mesh2D>();
  mesh->set_vertices(std::vector<glm::vec2>{
    { 0.5f, 0.5f },
    { 0.5f, -0.5f },
    { -0.5f, -0.5f },
    { -0.5f, 0.5f },
  });
  mesh->set_uv(std::vector<glm::vec2>{
    { 1.0f, 1.0f },
    { 1.0f, 0.0f },
    { 0.0f, 0.0f },
    { 0.0f, 1.0f },
  });
  mesh->set_indices(std::vector<uint32_t>{ 0, 1, 2, 0, 2, 3 });
  mesh->add_texture(axgl::interface::Texture::DIFFUSE, texture);
  mesh->scale = glm::vec3(200.0f);

  // triangle entity
  auto entity = realm->create_entity();
  entity->add_component(mesh);

  axgl.run();
}
