#include <../../../axgl/include/axgl/impl/services/camera_service.hpp>
#include <../../../axgl/include/axgl/impl/services/resource_service.hpp>
#include <../../../impl/bundlefile/include/axgl/impl/services/bundlefile_service.hpp>
#include <axgl/axgl.hpp>
#include <axgl/impl/assimp.hpp>
#include <axgl/impl/components/camera.hpp>
#include <axgl/impl/components/light.hpp>
#include <axgl/impl/glfw.hpp>
#include <axgl/impl/opengl.hpp>
#include <axgl/impl/realm_service.hpp>
#include <axgl/impl/service.hpp>

class Application final : public axgl::impl::ServiceBase
{
public:
  void initialize() override
  {
    const auto axgl = get_context()->axgl;

    // window
    const auto window = axgl->window_service()->create_window();
    window->set_title("Hello model!");

    // input
    axgl->input_service()->set_window(window);

    // renderer
    const auto renderer_service = axgl->renderer_service();
    const auto renderer = renderer_service->create_renderer();
    renderer->set_window(window);

    // realm
    const auto realm_service = axgl->realm_service();
    const auto realm = axgl->realm_service()->create_realm();
    realm->set_renderer(renderer);

    // load bundlefile
    const auto bundlefile_service = axgl->get_service<axgl::impl::BundlefileService>("bundlefile");
    bundlefile_service->load_bundlefile("demo_opengl_model_res.bin");

    // model entity
    const auto entity = realm_service->create_entity<axgl::interface::Entity>();
    axgl->model_service()->load_model(entity, "backpack.assbin");
    entity->transform()->scale = glm::vec3(10);
    entity->update_model_matrix();
    realm->add_entity(entity);

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

    // camera input
    const auto camera_service = axgl->get_service<axgl::impl::CameraService>("camera");
    camera_service->set_camera_mode(std::make_shared<axgl::impl::Keyboard3DFreeFlyCameraMode>());
    camera_service->set_camera(camera_entity);
  }
};

int main()
{
  axgl::Axgl axgl;
  axgl.use_service<axgl::impl::RealmService>();
  axgl.use_service<axgl::impl::CameraService>();
  axgl.use_service<axgl::impl::ResourceService>();
  axgl.use_service<axgl::impl::BundlefileService>();
  axgl.use_service<axgl::impl::GlfwInputService>();
  axgl.use_service<axgl::impl::GlfwWindowService>();
  axgl.use_service<axgl::impl::OpenglRendererService>();
  axgl.use_service<axgl::impl::AssimpModelService>();
  axgl.register_service("app", std::make_shared<Application>());
  axgl.run();
}
