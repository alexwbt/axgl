#include <axgl/axgl.hpp>
#ifdef AXGL_DEMO_USE_OPENGL_IMPL
  #include <axgl/impl/glfw.hpp>
  #include <axgl/impl/opengl.hpp>
#endif
#include <axgl/impl/camera/keyboard_3d_free_fly_camera_mode.hpp>
#include <axgl/util/mesh.hpp>

#include <demo_text/res.hpp>

class Application final : public axgl::Service
{
public:
  void on_start(const Context& context) override
  {
    const auto& window_service = context.axgl.window_service();
    const auto& input_service = context.axgl.input_service();
    const auto& renderer_service = context.axgl.renderer_service();
    const auto& realm_service = context.axgl.realm_service();
    const auto& entity_service = context.axgl.entity_service();
    const auto& camera_service = context.axgl.camera_service();
    const auto& text_service = context.axgl.text_service();

    // window
    const auto window = window_service->create_window();
    window->set_title("Hello text!");

    // input
    input_service->set_window(window);

    // renderer
    const auto renderer = renderer_service->create_renderer();
    renderer->set_window(window);
    renderer->set_sample_count(8);
    renderer->set_antialiasing(true);
    renderer_service->set_active_renderer(renderer);

    // realm
    const auto realm = realm_service->create_default_realm();
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
      light_comp->light.color.ambient = glm::vec3(0.3f);
      light_comp->light.direction = glm::vec3(0.2f, -1.0f, 1.2f);
      light_entity->components().add(light_comp);
      realm->entities().add(light_entity);
    }

    // load fonts
    text_service->load_font("arial", demo_text_res::get("font/arial.ttf"), 0);
    text_service->load_font("noto-tc", demo_text_res::get("font/noto-tc.ttf"), 0);

    // text entity
    {
      // texture
      const auto texture = text_service->create_texture({
        .value = reinterpret_cast<const char*>(
          u8"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 !@#$%^&*()[]{}<>,.`~-_+=\\/|?'\":;"
          u8"蒙沙新書章節論；附【優價】電影放映。學校商店：千手藝伎百科全書《長屋齋梶地寺大急平町地區大村》。"),
        .fonts = {"arial", "noto-tc"},
        .font_color = {1.0f, 0.5f, 0.2f, 1.0f},
        .font_size = 48.0f,
      });
      // material
      const auto material = renderer_service->create_material("2d");
      material->add_texture(axgl::Material::TextureType::kDiffuse, texture);
      // mesh
      const auto mesh = entity_service->create_component_t<axgl::component::Mesh>();
      axgl::util::init_quad(*mesh);
      mesh->set_material(material);
      // entity
      const auto text_entity = entity_service->create_entity();
      text_entity->components().add(mesh);
      text_entity->transform().scale = glm::vec3(texture->get_width(), texture->get_height(), 1.0f) * 0.01f;
      text_entity->update_model_matrix();
      realm->entities().add(text_entity);
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
  axgl.register_service("app", axgl::create_ptr<Application>());
  axgl.initialize();

  axgl.run();
  axgl.terminate();
}
