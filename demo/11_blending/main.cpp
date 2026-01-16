#include <axgl/axgl.hpp>
#ifdef AXGL_DEMO_USE_OPENGL_IMPL
  #include <axgl/impl/glfw.hpp>
  #include <axgl/impl/opengl.hpp>
#endif
#include <axgl/impl/camera/keyboard_3d_free_fly_camera_mode.hpp>
#include <axgl/util/mesh.hpp>

auto create_cube(const axgl::Axgl& axgl, const glm::vec3& position, const glm::vec4& color, const bool blend)
{
  const auto entity_service = axgl.entity_service();
  const auto renderer_service = axgl.renderer_service();
  // material
  const auto material = renderer_service->create_material("blinn-phong");
  material->set_color(color);
  if (blend)
  {
    material->set_enable_blend(true);
    material->set_property("shininess", "0");
  }
  // mesh
  const auto mesh = entity_service->create_component_t<axgl::component::Mesh>();
  axgl::util::init_cube(*mesh);
  mesh->set_material(material);
  // cube entity
  const auto cube = axgl.entity_service()->create_entity();
  cube->components().add(mesh);
  cube->transform().position = position;
  cube->update_model_matrix();
  return cube;
}

class Application final : public axgl::Service
{
public:
  void on_start(const Context& context) override
  {
    const auto& axgl = context.axgl;
    const auto& window_service = axgl.window_service();
    const auto& input_service = axgl.input_service();
    const auto& renderer_service = axgl.renderer_service();
    const auto& realm_service = axgl.realm_service();
    const auto& entity_service = axgl.entity_service();
    const auto& camera_service = axgl.camera_service();

    // window
    const auto window = window_service->create_window();
    window->set_title("Hello blending!");

    // input
    input_service->set_window(window);

    // renderer
    const auto renderer = renderer_service->create_renderer();
    renderer->set_window(window);
    renderer->set_sample_count(8);
    renderer->set_antialiasing(true);
    renderer_service->set_active_renderer(renderer);

    // realm
    const auto realm = realm_service->create_default_realm();
    realm_service->set_active_realm(realm);

    // camera entity
    {
      const auto camera_entity = entity_service->create_entity();
      const auto camera_comp = entity_service->create_component_t<axgl::impl::component::Camera>();
      camera_entity->components().add(camera_comp);
      realm->entities().add(camera_entity);
      camera_entity->transform().position.z = -2;

      // camera input
      camera_service->set_camera_mode(axgl::create_ptr<axgl::impl::camera::Keyboard3DFreeFlyCameraMode>());
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

    // entities
    realm->entities().add(create_cube(axgl, {0.3f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.5f}, true));
    realm->entities().add(create_cube(axgl, {0.0f, 0.3f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.5f}, true));
    realm->entities().add(create_cube(axgl, {0.0f, 0.0f, 0.3f}, {0.0f, 0.0f, 1.0f, 0.5f}, true));
    realm->entities().add(create_cube(axgl, {0.0f, 0.0f, 1.5f}, {1.0f, 0.5f, 0.2f, 1.0f}, false));
  }
};

int main()
{
  axgl::Axgl axgl;
  axgl::configure_default(axgl);
#ifdef AXGL_DEMO_USE_OPENGL_IMPL
  axgl::configure_glfw(axgl);
  axgl::configure_opengl(axgl);
#endif
  axgl.register_service("app", axgl::create_ptr<Application>());
  axgl.initialize();

  axgl.run();
  axgl.terminate();
}
