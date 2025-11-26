#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/glfw.hpp>
#include <axgl/impl/opengl.hpp>

static std::vector cube_vertices = {
  glm::vec3(-0.5f, -0.5f, -0.5f),
  glm::vec3(00.5f, 0.5f, -0.5f),
  glm::vec3(00.5f, -0.5f, -0.5f),
  glm::vec3(00.5f, 0.5f, -0.5f),
  glm::vec3(-0.5f, -0.5f, -0.5f),
  glm::vec3(-0.5f, 0.5f, -0.5f),
  glm::vec3(-0.5f, -0.5f, 0.5f),
  glm::vec3(00.5f, -0.5f, 0.5f),
  glm::vec3(00.5f, 0.5f, 0.5f),
  glm::vec3(00.5f, 0.5f, 0.5f),
  glm::vec3(-0.5f, 0.5f, 0.5f),
  glm::vec3(-0.5f, -0.5f, 0.5f),
  glm::vec3(-0.5f, 0.5f, 0.5f),
  glm::vec3(-0.5f, 0.5f, -0.5f),
  glm::vec3(-0.5f, -0.5f, -0.5f),
  glm::vec3(-0.5f, -0.5f, -0.5f),
  glm::vec3(-0.5f, -0.5f, 0.5f),
  glm::vec3(-0.5f, 0.5f, 0.5f),
  glm::vec3(00.5f, 0.5f, 0.5f),
  glm::vec3(00.5f, -0.5f, -0.5f),
  glm::vec3(00.5f, 0.5f, -0.5f),
  glm::vec3(00.5f, -0.5f, -0.5f),
  glm::vec3(00.5f, 0.5f, 0.5f),
  glm::vec3(00.5f, -0.5f, 0.5f),
  glm::vec3(-0.5f, -0.5f, -0.5f),
  glm::vec3(00.5f, -0.5f, -0.5f),
  glm::vec3(00.5f, -0.5f, 0.5f),
  glm::vec3(00.5f, -0.5f, 0.5f),
  glm::vec3(-0.5f, -0.5f, 0.5f),
  glm::vec3(-0.5f, -0.5f, -0.5f),
  glm::vec3(-0.5f, 0.5f, -0.5f),
  glm::vec3(00.5f, 0.5f, 0.5f),
  glm::vec3(00.5f, 0.5f, -0.5f),
  glm::vec3(00.5f, 0.5f, 0.5f),
  glm::vec3(-0.5f, 0.5f, -0.5f),
  glm::vec3(-0.5f, 0.5f, 0.5f),
};

static std::vector cube_normals = {
  glm::vec3(0.0f, 0.0f, -1.0f),
  glm::vec3(0.0f, 0.0f, -1.0f),
  glm::vec3(0.0f, 0.0f, -1.0f),
  glm::vec3(0.0f, 0.0f, -1.0f),
  glm::vec3(0.0f, 0.0f, -1.0f),
  glm::vec3(0.0f, 0.0f, -1.0f),
  glm::vec3(0.0f, 0.0f, 1.0f),
  glm::vec3(0.0f, 0.0f, 1.0f),
  glm::vec3(0.0f, 0.0f, 1.0f),
  glm::vec3(0.0f, 0.0f, 1.0f),
  glm::vec3(0.0f, 0.0f, 1.0f),
  glm::vec3(0.0f, 0.0f, 1.0f),
  -glm::vec3(1.0f, 0.0f, 0.0f),
  -glm::vec3(1.0f, 0.0f, 0.0f),
  -glm::vec3(1.0f, 0.0f, 0.0f),
  -glm::vec3(1.0f, 0.0f, 0.0f),
  -glm::vec3(1.0f, 0.0f, 0.0f),
  -glm::vec3(1.0f, 0.0f, 0.0f),
  glm::vec3(1.0f, 0.0f, 0.0f),
  glm::vec3(1.0f, 0.0f, 0.0f),
  glm::vec3(1.0f, 0.0f, 0.0f),
  glm::vec3(1.0f, 0.0f, 0.0f),
  glm::vec3(1.0f, 0.0f, 0.0f),
  glm::vec3(1.0f, 0.0f, 0.0f),
  glm::vec3(0.0f, -1.0f, 0.0f),
  glm::vec3(0.0f, -1.0f, 0.0f),
  glm::vec3(0.0f, -1.0f, 0.0f),
  glm::vec3(0.0f, -1.0f, 0.0f),
  glm::vec3(0.0f, -1.0f, 0.0f),
  glm::vec3(0.0f, -1.0f, 0.0f),
  glm::vec3(0.0f, 1.0f, 0.0f),
  glm::vec3(0.0f, 1.0f, 0.0f),
  glm::vec3(0.0f, 1.0f, 0.0f),
  glm::vec3(0.0f, 1.0f, 0.0f),
  glm::vec3(0.0f, 1.0f, 0.0f),
  glm::vec3(0.0f, 1.0f, 0.0f),
};

class Application final : public axgl::impl::ServiceBase
{
  std::shared_ptr<axgl::Entity> cube_entity_;

public:
  void on_start() override
  {
    const auto axgl = get_context()->axgl;

    // window
    const auto window = axgl->window_service()->create_window();
    window->set_title("Hello cube!");

    // renderer
    const auto renderer_service = axgl->renderer_service();
    const auto renderer = renderer_service->create_renderer();
    renderer->set_window(window);

    // realm
    const auto realm_service = axgl->realm_service();
    const auto realm = realm_service->create_realm();
    realm->set_renderer(renderer);

    // cube entity
    const auto entity_service = axgl->entity_service();
    cube_entity_ = entity_service->create_entity();
    {
      // material
      const auto material = renderer_service->create_material("default");
      material->set_color({1.0f, 0.5f, 0.2f, 1.0f});

      // cube mesh
      const auto mesh_comp = entity_service->create_component_t<axgl::component::Mesh>();
      mesh_comp->set_vertices(cube_vertices);
      mesh_comp->set_normals(cube_normals);
      mesh_comp->set_material(material);
      cube_entity_->components()->add(mesh_comp);
    }
    realm->add_entity(cube_entity_);

    // camera entity
    const auto camera_entity = entity_service->create_entity();
    {
      const auto camera_comp = entity_service->create_component_t<axgl::impl::component::Camera>();
      camera_entity->components()->add(camera_comp);
    }
    camera_entity->transform()->position.z = -2;
    realm->add_entity(camera_entity);
    axgl->camera_service()->set_camera(camera_entity);

    // light entity
    const auto light_entity = entity_service->create_entity();
    {
      const auto light_comp = entity_service->create_component_t<axgl::impl::component::Light>();
      light_comp->light.color.ambient = glm::vec3(0.3f);
      light_entity->components()->add(light_comp);
    }
    light_entity->transform()->rotation = glm::vec3(0.2f, -1.0f, 1.2f);
    realm->add_entity(light_entity);
  }

  void tick() override { cube_entity_->transform()->rotation += glm::vec3(0.01f, 0.02f, 0.05f); }

  void update() override { cube_entity_->update_model_matrix(); }
};

int main()
{
  axgl::Axgl axgl;
  axgl.use_service<axgl::impl::GlfwWindowService>();
  axgl.use_service<axgl::impl::GlfwInputService>();
  axgl.use_service<axgl::impl::OpenglRendererService>();
  axgl.use_service<axgl::impl::RealmService>();
  axgl.use_service<axgl::impl::EntityService>();
  axgl.use_service<axgl::impl::CameraService>();
  axgl.use_service<axgl::impl::LightService>();
  axgl.register_service("app", std::make_shared<Application>());
  axgl.initialize();

  axgl.run();
  axgl.terminate();
}
