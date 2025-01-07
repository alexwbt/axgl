#include <axgl/axgl.hpp>
#include <axgl/impl/glfw.hpp>
#include <axgl/impl/opengl.hpp>
#include <axgl/impl/realm_service.hpp>
#include <axgl/impl/camera_service.hpp>
#include <axgl/impl/resource_service.hpp>

#include <demo_opengl_gui/res.hpp>

class Application : public axgl::interface::Service
{
public:
  void initialize() override
  {
    auto axgl = get_context()->axgl;

    // window
    auto window = axgl->window_service()->create_window();
    window->set_title("Hello gui!");

    // renderer
    auto renderer_service = axgl->renderer_service();
    auto renderer = renderer_service->create_renderer();
    renderer->set_window(window);

    // resources
    auto resource_service = axgl->resource_service();
    resource_service->load_resources(demo_opengl_gui_res::data);

    // realm
    auto realm_service = axgl->realm_service();
    auto realm = axgl->realm_service()->create_realm();
    realm->set_renderer(renderer);

    // camera
    realm->camera.position.z = -2;
    realm->camera.update();
    axgl->input_service()->set_window(window);
    axgl->get_service<axgl::impl::CameraService>("camera")
      ->set_camera_mode(std::make_shared<axgl::impl::Keyboard3DFreeFlyCameraMode>());

    // light
    realm->lights.emplace_back(glm::vec3(0.2f, -1.0f, 1.2f),
      axgl::interface::Light::Color{ glm::vec3(0.3f), glm::vec3(1), glm::vec3(1) });

    // gui
    auto gui_service = axgl->gui_service();

    auto text = gui_service->create_element();
    text->props.font = "arial,noto-tc";
    text->props.content = (const char*)
      u8"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 !@#$%^&*()[]{}<>,.`~-_+=\\/|?'\":;"
      u8"蒙沙新書章節論；附【優價】電影放映。學校商店：千手藝伎百科全書《長屋齋梶地寺大急平町地區大村》。";
    text->props.font_size = 48;
    text->props.size = glm::vec2(200, 200);
    text->props.bg_color = glm::vec4(1.0f, 0.5f, 0.2f, 1.0f);

    auto page = gui_service->create_page();
    page->props.size = window->get_size();
    page->props.bg_color = glm::vec4(0.2f, 0.5f, 0.2f, 1.0f);
    page->add_child(text);
    realm->add_component(page);
  }
};

int main()
{
  axgl::Axgl axgl;
  axgl.use_service<axgl::impl::RealmService>();
  axgl.use_service<axgl::impl::CameraService>();
  axgl.use_service<axgl::impl::ResourceService>();
  axgl.use_service<axgl::impl::GlfwInputService>();
  axgl.use_service<axgl::impl::GlfwWindowService>();
  axgl.use_service<axgl::impl::OpenglRendererService>();
  axgl.use_service<axgl::impl::OpenglGuiService>();
  axgl.register_service("app", std::make_shared<Application>());
  axgl.run();
}
