#include <axgl/axgl.hpp>
#ifdef AXGL_DEMO_USE_OPENGL_IMPL
  #include <axgl/impl/glfw.hpp>
  #include <axgl/impl/opengl.hpp>
#endif
#include <axgl/common/mesh.hpp>
#include <axgl/impl/bullet.hpp>

class Application final : public axgl::impl::ServiceBase {
public:
  static constexpr std::string_view kTypeId = "app";

private:
  std::shared_ptr<axgl::Entity> cube_entity_;

public:
  void on_start() override {
    const auto& window_service = axgl_->window_service();
    const auto& renderer_service = axgl_->renderer_service();
    const auto& realm_service = axgl_->realm_service();
    const auto& entity_service = axgl_->entity_service();
    const auto& camera_service = axgl_->camera_service();
    const auto& physics_service = axgl_->physics_service();

    // window
    const auto window = window_service->create_window();
    window->set_title("Bullet physics");

    // renderer
    const auto renderer = renderer_service->create_renderer();
    renderer->set_window(window);
    renderer->set_enable_msaa(true);
    renderer->set_msaa_sample_count(8);
    renderer_service->set_active_renderer(renderer);

    // realm
    const auto realm = realm_service->create_realm();
    realm_service->set_active_realm(realm);

    // camera
    {
      const auto camera_entity = entity_service->create_entity();
      const auto camera_comp
        = entity_service->create_component_t<axgl::impl::component::Camera>();
      camera_entity->add_component(camera_comp);
      realm->entities().add(camera_entity);
      camera_entity->transform().position.z = -8;
      camera_entity->transform().position.y = 3;
      camera_entity->transform().update_matrix();
      camera_service->set_camera_entity(camera_entity);
    }

    // light
    {
      const auto light_entity = entity_service->create_entity();
      const auto light_comp
        = entity_service->create_component_t<axgl::impl::component::Light>();
      light_comp->light = axgl::Light::sunlight({0.2f, -1.0f, 1.2f});
      light_entity->add_component(light_comp);
      realm->entities().add(light_entity);
    }

    // static ground
    {
      const auto ground_entity = entity_service->create_entity();
      ground_entity->transform().scale = {10.0f, 1.0f, 10.0f};
      ground_entity->transform().position.y = -1.0f;
      ground_entity->transform().update_matrix();

      const auto material = renderer_service->create_material("3d");
      material->set_color({0.3f, 0.3f, 0.35f, 1.0f});
      const auto mesh_comp
        = entity_service->create_component_t<axgl::component::Mesh>();
      axgl::mesh::init_cube(*mesh_comp);
      mesh_comp->set_material(material);
      ground_entity->add_component(mesh_comp);

      const auto shape
        = physics_service->create_shape_t<axgl::impl::bullet::BoxShape>();
      shape->set_half_extents({5.0f, 0.5f, 5.0f});
      const auto body
        = entity_service
            ->create_component_t<axgl::impl::bullet::component::RigidBody>();
      body->set_shape(shape);
      body->set_mass(0.0f);
      ground_entity->add_component(body);

      realm->entities().add(ground_entity);
    }

    // dynamic falling cube
    {
      cube_entity_ = entity_service->create_entity();
      cube_entity_->transform().position.y = 6.0f;
      cube_entity_->transform().update_matrix();

      const auto material = renderer_service->create_material("3d");
      material->set_color({1.0f, 0.5f, 0.2f, 1.0f});
      const auto mesh_comp
        = entity_service->create_component_t<axgl::component::Mesh>();
      axgl::mesh::init_cube(*mesh_comp);
      mesh_comp->set_material(material);
      cube_entity_->add_component(mesh_comp);

      const auto shape
        = physics_service->create_shape_t<axgl::impl::bullet::BoxShape>();
      shape->set_half_extents({0.5f, 0.5f, 0.5f});
      const auto body
        = entity_service
            ->create_component_t<axgl::impl::bullet::component::RigidBody>();
      body->set_shape(shape);
      body->set_mass(1.0f);
      body->set_restitution(0.4f);
      cube_entity_->add_component(body);

      realm->entities().add(cube_entity_);
    }
  }
};

int main() {
  axgl::Axgl axgl;
  axgl::configure_default(axgl);
#ifdef AXGL_DEMO_USE_OPENGL_IMPL
  axgl::configure_glfw(axgl);
  axgl::configure_opengl(axgl);
#endif
  axgl::configure_bullet(axgl);
  axgl.register_service_t<Application>();
  axgl.initialize();

  axgl.run();
  axgl.terminate();
}
