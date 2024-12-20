
#include <vector>

#define AXGL_DEBUG
#include <axgl/axgl.hpp>
#include <axgl/impl/glfw.hpp>
#include <axgl/impl/opengl.hpp>
#include <axgl/impl/realm_service.hpp>

#include <demo_opengl_texture/res.hpp>

int main()
{
  axgl::Axgl axgl;
  axgl.use_service<axgl::impl::GlfwWindowService>();
  axgl.use_service<axgl::impl::OpenglRendererService>();
  axgl.use_service<axgl::impl::RealmService>();

  // window
  auto window = axgl.window_service()->create_window();
  window->set_title("Hello texture!");

  // renderer
  auto renderer_service = axgl.renderer_service();
  auto renderer = renderer_service->create_renderer();
  renderer->set_window(window);

  // realm
  auto realm_service = axgl.realm_service();
  auto realm = axgl.realm_service()->create_realm();
  realm->set_renderer(renderer);
  realm->camera.orthographic = true;
  realm->camera.near_clip = -1;
  realm->camera.far_clip = 1;

  // texture
  auto texture = renderer_service->create_texture();
  texture->load_texture(demo_opengl_texture_res::get("container.png"));

  // material
  auto material = renderer_service->create_material("2d");
  material->add_texture(axgl::interface::TextureType::DIFFUSE, texture);

  // square mesh
  auto mesh = realm_service->create_component<axgl::interface::Mesh>();
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
  mesh->set_material(material);
  mesh->scale = glm::vec3(200.0f);
  mesh->update_model_matrix();
  realm->add_component(mesh);

  axgl.run();
}
