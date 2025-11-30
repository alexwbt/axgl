
#include <axgl/axgl.hpp>
#include <axgl/impl/glfw.hpp>
#include <axgl/impl/opengl.hpp>

#include "cube_data.hpp"

class Application final : public axgl::Service
{
  std::shared_ptr<axgl::Entity> cube_entity_;

public:
  void on_start(const Context& context) override
  {
    const auto axgl = context.axgl;
    const auto window_service = axgl.window_service();
    const auto renderer_service = axgl.renderer_service();
    const auto realm_service = axgl.realm_service();
    const auto entity_service = axgl.entity_service();
    const auto camera_service = axgl.camera_service();

    // window
    const auto window = window_service->create_window();
    window->set_title("Hello cube!");

    // renderer
    const auto renderer = renderer_service->create_renderer();
    renderer->set_window(window);

    // realm
    const auto realm = realm_service->create_realm();
    realm_service->set_active_realm(realm);
    realm->set_renderer(renderer);

    // camera entity
    const auto camera_entity = entity_service->create_entity();
    {
      const auto camera_comp = entity_service->create_component_t<axgl::impl::component::Camera>();
      camera_entity->components()->add(camera_comp);
    }
    realm->add_entity(camera_entity);
    camera_entity->transform().position.z = -2;
    camera_service->set_camera(camera_entity);

    // light entity
    const auto light_entity = entity_service->create_entity();
    {
      const auto light_comp = entity_service->create_component_t<axgl::impl::component::Light>();
      light_comp->light.color.ambient = glm::vec3(0.3f);
      light_entity->components()->add(light_comp);
    }
    light_entity->transform().rotation = glm::vec3(0.2f, -1.0f, 1.2f);
    realm->add_entity(light_entity);

    // cube entity
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
  }

  void tick(const Context& context) override { cube_entity_->transform().rotation += glm::vec3(0.01f, 0.02f, 0.05f); }

  void update(const Context& context) override { cube_entity_->update_model_matrix(); }
};

int main()
{
  axgl::Axgl axgl;
  axgl.register_service_t<axgl::impl::glfw::WindowService>();
  axgl.register_service_t<axgl::impl::glfw::InputService>();
  axgl.register_service_t<axgl::impl::opengl::RendererService>();
  axgl.register_service_t<axgl::impl::RealmService>();
  axgl.register_service_t<axgl::impl::EntityService>();
  axgl.register_service_t<axgl::impl::CameraService>();
  axgl.register_service_t<axgl::impl::LightService>();
  axgl.register_service("app", std::make_shared<Application>());
  axgl.initialize();

  axgl.run();
  axgl.terminate();
}
