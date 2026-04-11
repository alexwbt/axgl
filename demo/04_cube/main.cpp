#include <axgl/axgl.hpp>
#ifdef AXGL_DEMO_USE_OPENGL_IMPL
  #include <axgl/impl/glfw.hpp>
  #include <axgl/impl/opengl.hpp>
#endif
#include <axgl/util/mesh.hpp>

class Application final : public axgl::impl::ServiceBase
{
public:
  static constexpr std::string_view kTypeId = "app";

private:
  std::shared_ptr<axgl::Entity> cube_entity_;

public:
  void on_start() override
  {
    const auto& window_service = axgl_->window_service();
    const auto& renderer_service = axgl_->renderer_service();
    const auto& realm_service = axgl_->realm_service();
    const auto& entity_service = axgl_->entity_service();
    const auto& camera_service = axgl_->camera_service();

    // window
    const auto window = window_service->create_window();
    window->set_title("Hello cube!");

    // renderer
    const auto renderer = renderer_service->create_renderer();
    renderer->set_window(window);
    renderer->set_sample_count(8);
    renderer->set_antialiasing(true);
    renderer_service->set_active_renderer(renderer);

    // realm
    const auto realm = realm_service->create_realm();
    realm_service->set_active_realm(realm);

    // camera entity
    {
      const auto camera_entity = entity_service->create_entity();
      const auto camera_comp = entity_service->create_component_t<axgl::impl::component::Camera>();
      camera_entity->components().add(camera_comp);
      realm->entities().add(camera_entity);
      camera_entity->transform().position.z = -2;
      camera_service->set_camera_entity(camera_entity);
    }

    // light entity
    {
      const auto light_entity = entity_service->create_entity();
      const auto light_comp = entity_service->create_component_t<axgl::impl::component::Light>();
      light_comp->light = axgl::Light::sunlight({0.2f, -1.0f, 1.2f});
      light_entity->components().add(light_comp);
      realm->entities().add(light_entity);
    }

    // cube entity
    {
      cube_entity_ = entity_service->create_entity();
      // material
      const auto material = renderer_service->create_material("3d");
      material->set_color({1.0f, 0.5f, 0.2f, 1.0f});

      // cube mesh
      const auto mesh_comp = entity_service->create_component_t<axgl::component::Mesh>();
      axgl::util::init_cube(*mesh_comp);
      mesh_comp->set_material(material);
      cube_entity_->components().add(mesh_comp);
      realm->entities().add(cube_entity_);
    }
  }

  void tick() override { cube_entity_->transform().rotation += glm::vec3(0.01f, 0.02f, 0.05f); }
};

int main()
{
  axgl::Axgl axgl;
  axgl::configure_default(axgl);
#ifdef AXGL_DEMO_USE_OPENGL_IMPL
  axgl::configure_glfw(axgl);
  axgl::configure_opengl(axgl);
#endif
  axgl.register_service_t<Application>();
  axgl.initialize();

  axgl.run();
  axgl.terminate();
}
