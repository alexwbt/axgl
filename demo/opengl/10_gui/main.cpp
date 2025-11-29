
#include <axgl/interface/components/gui.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/camera_modes/keyboard_3d_free_fly_camera_mode.hpp>
#include <axgl/impl/glfw.hpp>
#include <axgl/impl/opengl.hpp>

#include <demo_opengl_gui/res.hpp>

class Application : public axgl::Service
{
public:
  void on_start(const Context& context) override
  {
    const auto axgl = context.axgl;
    const auto window_service = axgl.window_service();
    const auto input_service = axgl.input_service();
    const auto renderer_service = axgl.renderer_service();
    const auto realm_service = axgl.realm_service();
    const auto entity_service = axgl.entity_service();
    const auto camera_service = axgl.camera_service();
    const auto text_service = axgl.get_service<axgl::impl::opengl::TextService>("text");

    // window
    const auto window = window_service->create_window();
    window->set_title("Hello gui!");

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
      camera_entity->components()->add(camera_comp);
    }
    realm->add_entity(camera_entity);
    camera_entity->transform().position.z = -2;
    camera_service->set_camera(camera_entity);

    // light entity
    const auto light_entity = entity_service->create_entity();
    {
      const auto light_comp = entity_service->create_component_t<axgl::impl::component::Light>();
      light_comp->light.color.ambient = glm::vec3(0.3f);
      light_entity->components()->add(light_comp);
    }
    light_entity->transform().rotation = glm::vec3(0.2f, -1.0f, 1.2f);
    realm->add_entity(light_entity);

    // camera input
    camera_service->set_camera_mode(std::make_shared<axgl::impl::camera_modes::Keyboard3DFreeFlyCameraMode>());
    camera_service->set_camera(camera_entity);

    // load fonts
    text_service->load_font("arial", demo_opengl_gui_res::get("font/arial.ttf"));
    text_service->load_font("noto-tc", demo_opengl_gui_res::get("font/noto-tc.ttf"));

    // gui entity
    const auto gui_entity = entity_service->create_entity();
    {
      const auto gui_component = entity_service->create_component_t<axgl::component::Gui>();
      // gui_component->set_root(axgl::gui::element<axgl::gui::Element>());
    }
  }
};

int main()
{
  axgl::Axgl axgl;
  axgl.use_service<axgl::impl::RealmService>();
  axgl.use_service<axgl::impl::EntityService>();
  axgl.use_service<axgl::impl::CameraService>();
  axgl.use_service<axgl::impl::LightService>();
  axgl.use_service<axgl::impl::glfw::WindowService>();
  axgl.use_service<axgl::impl::glfw::InputService>();
  axgl.use_service<axgl::impl::opengl::RendererService>();
  axgl.use_service<axgl::impl::opengl::TextService>();
  axgl.register_service("app", std::make_shared<Application>());
  axgl.initialize();

  axgl.run();
  axgl.terminate();
}
