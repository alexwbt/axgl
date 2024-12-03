
#define AXGL_DEBUG
#include <axgl/axgl.hpp>
#include <axgl/impl/realm_service.hpp>
#include <axgl/impl/glfw/glfw_service.hpp>
#include <axgl/impl/opengl/opengl_service.hpp>

#include "demo_opengl_textured_cube/res.hpp"

static std::vector<glm::vec3> cube_vertices = {
  glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(00.5f, -0.5f, -0.5f), glm::vec3(00.5f,  0.5f, -0.5f), glm::vec3(00.5f,  0.5f, -0.5f), glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-0.5f, -0.5f, -0.5f),
  glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(00.5f,  0.5f,  0.5f), glm::vec3(00.5f, -0.5f,  0.5f), glm::vec3(00.5f,  0.5f,  0.5f), glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-0.5f,  0.5f,  0.5f),
  glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-0.5f, -0.5f,  0.5f),
  glm::vec3(00.5f,  0.5f,  0.5f), glm::vec3(00.5f,  0.5f, -0.5f), glm::vec3(00.5f, -0.5f, -0.5f), glm::vec3(00.5f, -0.5f, -0.5f), glm::vec3(00.5f, -0.5f,  0.5f), glm::vec3(00.5f,  0.5f,  0.5f),
  glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(00.5f, -0.5f,  0.5f), glm::vec3(00.5f, -0.5f, -0.5f), glm::vec3(00.5f, -0.5f,  0.5f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-0.5f, -0.5f,  0.5f),
  glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(00.5f,  0.5f, -0.5f), glm::vec3(00.5f,  0.5f,  0.5f), glm::vec3(00.5f,  0.5f,  0.5f), glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-0.5f,  0.5f, -0.5f),
};

static std::vector<glm::vec3> cube_normals = {
  glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f,  0.0f, -1.0f),
  glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f,  0.0f,  1.0f),
 -glm::vec3(1.0f,  0.0f,  0.0f),-glm::vec3(1.0f,  0.0f,  0.0f),-glm::vec3(1.0f,  0.0f,  0.0f),-glm::vec3(1.0f,  0.0f,  0.0f),-glm::vec3(1.0f,  0.0f,  0.0f),-glm::vec3(1.0f,  0.0f,  0.0f),
  glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(1.0f,  0.0f,  0.0f),
  glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f),
  glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  1.0f,  0.0f),
};

static std::vector<glm::vec2> cube_uv = {
  glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f),
  glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f),
  glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f),
  glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f),
  glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f),
  glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f),
};

class Application : public axgl::interface::Service
{
public:
  std::shared_ptr<axgl::interface::Mesh3D> mesh;

  void initialize() override
  {
    auto axgl = get_context()->axgl;

    // window
    auto window = axgl->window_service()->create_window();
    window->set_title("Hello textured cube!");

    // renderer
    auto renderer = axgl->renderer_service()->create_renderer();
    renderer->set_window(window);

    // realm
    auto realm = axgl->realm_service()->create_realm();
    realm->set_renderer(renderer);

    // camera
    realm->camera.position.z = -2;
    realm->camera.update();

    // diffuse texture
    auto diffuse_texture = renderer->create_texture();
    diffuse_texture->load_2d_texture(demo_opengl_textured_cube_res::get("container/diffuse.png"));
    // specular texture
    auto specular_texture = renderer->create_texture();
    specular_texture->load_2d_texture(demo_opengl_textured_cube_res::get("container/specular.png"));

    // square mesh
    mesh = axgl->create_component<axgl::interface::Mesh3D>();
    mesh->set_vertices(cube_vertices);
    mesh->set_normals(cube_normals);
    mesh->set_uv(cube_uv);
    mesh->add_texture(axgl::interface::Texture::DIFFUSE, diffuse_texture);
    mesh->add_texture(axgl::interface::Texture::SPECULAR, specular_texture);

    // cube entity
    auto entity = realm->create_entity();
    entity->add_component(mesh);

    // light
    realm->lights.emplace_back(glm::vec3(0.2f, -1.0f, 1.2f),
      axgl::interface::Light::Color(glm::vec3(0.3f), glm::vec3(1), glm::vec3(1)));
  }

  void update() override
  {
    mesh->rotation += glm::vec3(0.01f, 0.02f, 0.05f);
  }
};

int main()
{
  axgl::Axgl axgl;
  axgl.use_service<axgl::impl::GlfwWindowService>();
  axgl.use_service<axgl::impl::OpenglRendererService>();
  axgl.use_service<axgl::impl::DefaultRealmService>();
  axgl.register_service("app", std::make_shared<Application>());
  axgl.run();
}
