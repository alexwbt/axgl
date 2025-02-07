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

    // renderer
    auto renderer_service = axgl->renderer_service();
    auto renderer = renderer_service->create_renderer();
    renderer->set_window(window);

    // realm
    auto realm_service = axgl->realm_service();
    auto realm = axgl->realm_service()->create_realm();
    realm->set_renderer(renderer);

    // camera
    axgl->input_service()->set_window(window);
    realm->camera.position.z = -20;
    realm->camera.update();
    auto camera_service = axgl->get_service<axgl::impl::CameraService>("camera");
    camera_service->set_camera_mode(std::make_shared<axgl::impl::Keyboard3DFreeFlyCameraMode>());

    // load bundlefile
    auto bundlefile_service = axgl->get_service<axgl::impl::BundlefileService>("bundlefile");
    bundlefile_service->load_bundlefile("demo_opengl_model_res.bin");

    // load model
    auto model = realm_service->create_component<axgl::interface::Component>();
    axgl->model_service()->load_model(model, "backpack.assbin");
    model->scale = glm::vec3(10);
    model->update_model_matrix();
    realm->add_component(model);

    // light
    realm->lights.emplace_back(glm::vec3(0.2f, -1.0f, 1.2f),
      axgl::interface::Light::Color { glm::vec3(0.3f), glm::vec3(1), glm::vec3(1) });
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
