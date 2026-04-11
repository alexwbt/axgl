#include <axgl/axgl.hpp>
#include <axgl/impl/assimp.hpp>
#include <axgl/impl/bundlefile.hpp>
#ifdef AXGL_DEMO_USE_OPENGL_IMPL
  #include <axgl/impl/glfw.hpp>
  #include <axgl/impl/opengl.hpp>
#endif
#include <axgl/impl/camera/keyboard_3d_free_fly_camera_mode.hpp>

class Application final : public axgl::impl::ServiceBase
{
public:
  static constexpr std::string_view kTypeId = "app";

  void on_start() override
  {
    const auto& window_service = axgl_->window_service();
    const auto& input_service = axgl_->input_service();
    const auto& renderer_service = axgl_->renderer_service();
    const auto& realm_service = axgl_->realm_service();
    const auto& entity_service = axgl_->entity_service();
    const auto& camera_service = axgl_->camera_service();
    const auto& model_service = axgl_->model_service();
    const auto& bundlefile_service = axgl_->get_service_t<axgl::impl::bundlefile::BundlefileService>();

    // window
    const auto window = window_service->create_window();
    window->set_title("Hello model!");

    // input
    input_service->set_window(window);

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

    // load bundlefile
    bundlefile_service->load_bundlefile("demo_model_res.bin");

    // model entity
    {
      const auto entity = entity_service->create_entity();
      const auto model_resources = model_service->load_model("backpack.glb");
      auto& components = entity->components();
      for (const auto& mesh : model_resources.meshes)
        components.add(mesh);

      entity->transform().scale = glm::vec3(10);
      realm->entities().add(entity);
    }
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
  axgl::configure_assimp(axgl);
  axgl::configure_bundlefile(axgl);
  axgl.register_service_t<Application>();
  axgl.initialize();

  axgl.run();
  axgl.terminate();
}
