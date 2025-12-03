
#include <axgl/axgl.hpp>
#include <axgl/impl/glfw.hpp>
#include <axgl/impl/opengl.hpp>

#include <axgl/impl/camera_modes/keyboard_3d_free_fly_camera_mode.hpp>

#include <demo_opengl_text/res.hpp>

class Application : public axgl::Service
{
public:
  void on_start(const Context& context) override
  {
    const auto window_service = context.axgl.window_service();
    const auto input_service = context.axgl.input_service();
    const auto renderer_service = context.axgl.renderer_service();
    const auto realm_service = context.axgl.realm_service();
    const auto entity_service = context.axgl.entity_service();
    const auto camera_service = context.axgl.camera_service();
    const auto text_service = context.axgl.get_service_t<axgl::impl::opengl::TextService>();

    // window
    const auto window = window_service->create_window();
    window->set_title("Hello text!");

    // input
    input_service->set_window(window);

    // renderer
    const auto renderer = renderer_service->create_renderer();
    renderer->set_window(window);
    renderer_service->set_active_renderer(renderer);

    // realm
    const auto realm = realm_service->create_realm();
    realm_service->set_active_realm(realm);

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

    // load fonts
    text_service->load_font("arial", demo_opengl_text_res::get("font/arial.ttf"));
    text_service->load_font("noto-tc", demo_opengl_text_res::get("font/noto-tc.ttf"));

    // text entity
    const auto text_entity = text_service->create_entity(
      reinterpret_cast<const char*>(
        u8"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 !@#$%^&*()[]{}<>,.`~-_+=\\/|?'\":;"
        u8"蒙沙新書章節論；附【優價】電影放映。學校商店：千手藝伎百科全書《長屋齋梶地寺大急平町地區大村》。"),
      {"arial", "noto-tc"}, {.size = 48, .color = {1.0f, 0.5f, 0.2f, 1.0f}}, 0.01f);
    realm->entities().add(text_entity);
  }
};

int main()
{
  axgl::Axgl axgl;
  axgl::configure_default(axgl);
  axgl::configure_glfw(axgl);
  axgl::configure_opengl(axgl);
  axgl.register_service("app", std::make_shared<Application>());
  axgl.initialize();

  axgl.run();
  axgl.terminate();
}
