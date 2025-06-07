#include <axgl/axgl.hpp>
#include <axgl/impl/glfw.hpp>
#include <axgl/impl/opengl.hpp>
#include <axgl/impl/assimp.hpp>
#include <axgl/impl/realm_service.hpp>
#include <axgl/impl/camera_service.hpp>
#include <axgl/impl/resource_service.hpp>
#include <axgl/impl/bundlefile_service.hpp>

class Application : public axgl::interface::Service
{
public:
  void initialize() override
  {
    auto axgl = get_context()->axgl;

    // window
    auto window = axgl->window_service()->create_window();
    window->set_title("Hello model!");

    // input
    axgl->input_service()->set_window(window);

    // renderer
    auto renderer_service = axgl->renderer_service();
    auto renderer = renderer_service->create_renderer();
    renderer->set_window(window);
    renderer->camera.position.z = -20;
    renderer->camera.update();
    renderer->lights.emplace_back(glm::vec3(0.2f, -1.0f, 1.2f),
      axgl::interface::Light::Color{ glm::vec3(0.3f), glm::vec3(1), glm::vec3(1) });

    // realm
    auto realm_service = axgl->realm_service();
    auto realm = axgl->realm_service()->create_realm();
    realm->set_renderer(renderer);

    // camera
    auto camera_service = axgl->get_service<axgl::impl::CameraService>("camera");
    camera_service->set_camera_mode(std::make_shared<axgl::impl::Keyboard3DFreeFlyCameraMode>());
    camera_service->set_renderer(renderer);

    // load bundlefile
    auto bundlefile_service = axgl->get_service<axgl::impl::BundlefileService>("bundlefile");
    bundlefile_service->load_bundlefile("demo_opengl_model_res.bin");

    // model entity
    auto entity = realm_service->create_entity<axgl::interface::Entity>();
    axgl->model_service()->load_model(entity, "backpack.assbin");
    entity->scale = glm::vec3(10);
    entity->update_model_matrix();
    realm->add_entity(entity);
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
