
#include <axgl/axgl.hpp>
#include <axgl/impl/assimp.hpp>
#include <axgl/impl/bundlefile.hpp>
#include <axgl/impl/glfw.hpp>
#include <axgl/impl/opengl.hpp>

class Application final : public axgl::Service
{
public:
  void initialize(const Context& context) override
  {
    const auto axgl = context.axgl;
    const auto window_service = axgl.window_service();
    const auto input_service = axgl.input_service();
    const auto renderer_service = axgl.renderer_service();
    const auto realm_service = axgl.realm_service();
    const auto entity_service = axgl.entity_service();
    const auto camera_service = axgl.camera_service();

    // window
    const auto window = window_service->create_window();
    window->set_title("Hello model!");

    // input
    input_service->set_window(window);

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
      camera_entity->components().add(camera_comp);
    }
    realm->entities().add(camera_entity);
    camera_entity->transform().position.z = -2;
    camera_service->set_camera(camera_entity);

    // light entity
    const auto light_entity = entity_service->create_entity();
    {
      const auto light_comp = entity_service->create_component_t<axgl::impl::component::Light>();
      light_comp->light.color.ambient = glm::vec3(0.3f);
      light_entity->components().add(light_comp);
    }
    light_entity->transform().rotation = glm::vec3(0.2f, -1.0f, 1.2f);
    realm->entities().add(light_entity);

    // camera input
    camera_service->set_camera_mode(std::make_shared<axgl::impl::camera_modes::Keyboard3DFreeFlyCameraMode>());
    camera_service->set_camera(camera_entity);

    // load bundlefile
    const auto bundlefile_service = axgl.get_service_t<axgl::impl::bundlefile::BundlefileService>();
    bundlefile_service->load_bundlefile("demo_opengl_model_res.bin");

    // model entity
    const auto entity = entity_service->create_entity();
    axgl.model_service()->load_model(entity, "backpack.assbin");
    entity->transform().scale = glm::vec3(10);
    entity->update_model_matrix();
    realm->entities().add(entity);
  }
};

int main()
{
  axgl::Axgl axgl;
  axgl::configure_default(axgl);
  axgl::configure_glfw(axgl);
  axgl::configure_opengl(axgl);
  axgl::configure_assimp(axgl);
  axgl::configure_bundlefile(axgl);
  axgl.register_service("app", std::make_shared<Application>());
  axgl.initialize();

  axgl.run();
  axgl.terminate();
}
