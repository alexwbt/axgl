
#define AXGL_DEBUG
#include <axgl/axgl.hpp>
#include <axgl/impl/realm_service.hpp>
#include <axgl/impl/camera_service.hpp>
#include <axgl/impl/glfw/glfw_service.hpp>
#include <axgl/impl/opengl/opengl_service.hpp>
#include <axgl/impl/assimp/assimp_model.hpp>

#include "demo_opengl_model/res.hpp"

class Application : public axgl::interface::Service
{
public:
  // std::shared_ptr<axgl::interface::Mesh3D> mesh;

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
    auto realm = axgl->realm_service()->create_realm();
    realm->set_renderer(renderer);

    // camera
    axgl->input_service()->set_window(window);
    realm->camera.position.z = -2;
    realm->camera.update();
    auto camera_service = axgl->get_service<axgl::impl::CameraService>("camera");
    camera_service->set_camera_mode(std::make_shared<axgl::impl::Keyboard3DFreeFlyCameraMode>());

    // entity
    auto entity = realm->create_entity();
    axgl->model_service()->load_model(entity,
      demo_opengl_model_res::get("iphone.fbx"));

    // light
    realm->lights.emplace_back(
      glm::vec3(0.2f, -1.0f, 1.2f),
      axgl::interface::Light::Color(
        glm::vec3(0.3f), glm::vec3(1), glm::vec3(1)));
  }

  void update() override
  {
    // mesh->rotation += glm::vec3(0.01f, 0.02f, 0.05f);
  }
};

int main()
{
  axgl::Axgl axgl;
  axgl.use_service<axgl::impl::GlfwInputService>();
  axgl.use_service<axgl::impl::GlfwWindowService>();
  axgl.use_service<axgl::impl::OpenglRendererService>();
  axgl.use_service<axgl::impl::AssimpModelService>();
  axgl.use_service<axgl::impl::RealmService>();
  axgl.use_service<axgl::impl::CameraService>();
  axgl.register_service("app", std::make_shared<Application>());
  axgl.run();
}
