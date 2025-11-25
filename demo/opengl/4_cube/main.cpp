
#include <../../../axgl/include/axgl/impl/services/realm_service.hpp>
#include <axgl/axgl.hpp>
#include <axgl/impl/component/camera.hpp>
#include <axgl/impl/component/light.hpp>
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
  std::shared_ptr<axgl::interface::Entity> cube_entity_;

public:
  void initialize() override
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
    cube_entity_ = realm_service->create_entity<axgl::interface::Entity>();
    {
      // material
      const auto material = renderer_service->create_material("default");
      material->set_color({1.0f, 0.5f, 0.2f, 1.0f});

      // cube mesh
      const auto mesh_comp = realm_service->create_component<axgl::interface::component::Mesh>();
      mesh_comp->set_vertices(cube_vertices);
      mesh_comp->set_normals(cube_normals);
      mesh_comp->set_material(material);
      cube_entity_->add_component(mesh_comp);
    }
    realm->add_entity(cube_entity_);

    // camera entity
    const auto camera_entity = realm_service->create_entity<axgl::interface::Entity>();
    {
      const auto camera_comp = realm_service->create_component<axgl::impl::component::Camera>();
      camera_entity->add_component(camera_comp);
    }
    camera_entity->transform()->position.z = -2;
    realm->add_entity(camera_entity);

    // light entity
    const auto light_entity = realm_service->create_entity<axgl::interface::Entity>();
    {
      const auto light_comp = realm_service->create_component<axgl::impl::component::Light>();
      light_comp->light.color.ambient = glm::vec3(0.3f);
      light_entity->add_component(light_comp);
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
  axgl.use_service<axgl::impl::OpenglRendererService>();
  axgl.use_service<axgl::impl::RealmService>();
  axgl.register_service("app", std::make_shared<Application>());
  axgl.run();
}
