
#include <axgl/axgl.hpp>
#include <axgl/impl/assimp.hpp>
#include <axgl/impl/bundlefile.hpp>
#include <axgl/impl/camera_modes/keyboard_3d_free_fly_camera_mode.hpp>
#include <axgl/impl/glfw.hpp>
#include <axgl/impl/opengl.hpp>

class Application final : public axgl::impl::ServiceBase
{
public:
  void initialize() override
  {
    const auto axgl = get_context()->axgl;
    const auto window_service = axgl->window_service();
    const auto input_service = axgl->input_service();
    const auto renderer_service = axgl->renderer_service();
    const auto realm_service = axgl->realm_service();
    const auto entity_service = axgl->entity_service();
    const auto camera_service = axgl->camera_service();

    // window
    const auto window = window_service->create_window();
    window->set_title("Hello model!");

    // input
    input_service->set_window(window);

    // renderer
    const auto renderer = renderer_service->create_renderer();
    renderer->set_window(window);

    // realm
    const auto realm = axgl->realm_service()->create_realm();
    realm->set_renderer(renderer);

    // camera entity
    const auto camera_entity = entity_service->create_entity();
    {
      const auto camera_comp = entity_service->create_component_t<axgl::impl::component::Camera>();
      camera_entity->components()->add(camera_comp);
    }
    realm->add_entity(camera_entity);
    camera_entity->transform()->position.z = -2;
    camera_service->set_camera(camera_entity);

    // light entity
    const auto light_entity = entity_service->create_entity();
    {
      const auto light_comp = entity_service->create_component_t<axgl::impl::component::Light>();
      light_comp->light.color.ambient = glm::vec3(0.3f);
      light_entity->components()->add(light_comp);
    }
    light_entity->transform()->rotation = glm::vec3(0.2f, -1.0f, 1.2f);
    realm->add_entity(light_entity);

    // camera input
    camera_service->set_camera_mode(std::make_shared<axgl::impl::camera_modes::Keyboard3DFreeFlyCameraMode>());
    camera_service->set_camera(camera_entity);

    // load bundlefile
    const auto bundlefile_service = axgl->get_service<axgl::impl::bundlefile::BundlefileService>("bundlefile");
    bundlefile_service->load_bundlefile("demo_opengl_model_res.bin");

    // model entity
    const auto entity = entity_service->create_entity();
    axgl->model_service()->load_model(entity, "backpack.assbin");
    entity->transform()->scale = glm::vec3(10);
    entity->update_model_matrix();
    realm->add_entity(entity);
  }
};

int main()
{
  axgl::Axgl axgl;
  axgl.use_service<axgl::impl::EntityService>();
  axgl.use_service<axgl::impl::LightService>();
  axgl.use_service<axgl::impl::RealmService>();
  axgl.use_service<axgl::impl::CameraService>();
  axgl.use_service<axgl::impl::ResourceService>();
  axgl.use_service<axgl::impl::bundlefile::BundlefileService>();
  axgl.use_service<axgl::impl::glfw::InputService>();
  axgl.use_service<axgl::impl::glfw::WindowService>();
  axgl.use_service<axgl::impl::opengl::RendererService>();
  axgl.use_service<axgl::impl::assimp::ModelService>();
  axgl.register_service("app", std::make_shared<Application>());
  axgl.initialize();

  axgl.run();
  axgl.terminate();
}
