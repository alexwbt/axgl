#include <axgl/axgl.hpp>
#include <axgl/impl/glfw.hpp>
#include <axgl/impl/opengl.hpp>
#include <axgl/impl/realm_service.hpp>
#include <axgl/impl/camera_service.hpp>
#include <axgl/impl/resource_service.hpp>
#include <axgl/impl/component/camera.hpp>
#include <axgl/impl/component/light.hpp>

#include <demo_opengl_gui/res.hpp>

static std::vector cube_vertices = {
  glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(00.5f,  0.5f, -0.5f), glm::vec3(00.5f, -0.5f, -0.5f), glm::vec3(00.5f,  0.5f, -0.5f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-0.5f,  0.5f, -0.5f),
  glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(00.5f, -0.5f,  0.5f), glm::vec3(00.5f,  0.5f,  0.5f), glm::vec3(00.5f,  0.5f,  0.5f), glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-0.5f, -0.5f,  0.5f),
  glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-0.5f,  0.5f,  0.5f),
  glm::vec3(00.5f,  0.5f,  0.5f), glm::vec3(00.5f, -0.5f, -0.5f), glm::vec3(00.5f,  0.5f, -0.5f), glm::vec3(00.5f, -0.5f, -0.5f), glm::vec3(00.5f,  0.5f,  0.5f), glm::vec3(00.5f, -0.5f,  0.5f),
  glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(00.5f, -0.5f, -0.5f), glm::vec3(00.5f, -0.5f,  0.5f), glm::vec3(00.5f, -0.5f,  0.5f), glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-0.5f, -0.5f, -0.5f),
  glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(00.5f,  0.5f,  0.5f), glm::vec3(00.5f,  0.5f, -0.5f), glm::vec3(00.5f,  0.5f,  0.5f), glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-0.5f,  0.5f,  0.5f),
};

static std::vector cube_normals = {
  glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f,  0.0f, -1.0f),
  glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f,  0.0f,  1.0f),
 -glm::vec3(1.0f,  0.0f,  0.0f),-glm::vec3(1.0f,  0.0f,  0.0f),-glm::vec3(1.0f,  0.0f,  0.0f),-glm::vec3(1.0f,  0.0f,  0.0f),-glm::vec3(1.0f,  0.0f,  0.0f),-glm::vec3(1.0f,  0.0f,  0.0f),
  glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(1.0f,  0.0f,  0.0f),
  glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f),
  glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  1.0f,  0.0f),
};

class Application final : public axgl::interface::Service
{
public:
  void initialize() override
  {
    const auto axgl = get_context()->axgl;

    // window
    const auto window = axgl->window_service()->create_window();
    window->set_title("Hello gui!");

    // input
    axgl->input_service()->set_window(window);

    // renderer
    const auto renderer_service = axgl->renderer_service();
    const auto renderer = renderer_service->create_renderer();
    renderer->set_window(window);

    // resources
    const auto resource_service = axgl->resource_service();
    resource_service->load_resources(demo_opengl_gui_res::data);

    // realm
    const auto realm_service = axgl->realm_service();
    const auto realm = axgl->realm_service()->create_realm();
    realm->set_renderer(renderer);

    // cube entity
    const auto cube = realm_service->create_entity<axgl::interface::Entity>();
    {
      // material
      const auto material = renderer_service->create_material("default");
      material->set_color({ 1.0f, 0.5f, 0.2f, 1.0f });

      // cube mesh
      const auto mesh = realm_service->create_component<axgl::interface::component::Mesh>();
      mesh->set_vertices(cube_vertices);
      mesh->set_normals(cube_normals);
      mesh->set_material(material);
      cube->add_component(mesh);
    }
    realm->add_entity(cube);

    // camera entity
    const auto camera_entity = realm_service->create_entity<axgl::interface::Entity>();
    {
      const auto camera_comp = realm_service->create_component<axgl::impl::component::Camera>();
      camera_entity->add_component(camera_comp);
    }
    camera_entity->position.z = -2;
    realm->add_entity(camera_entity);

    // light entity
    const auto light_entity = realm_service->create_entity<axgl::interface::Entity>();
    {
      const auto light_comp = realm_service->create_component<axgl::impl::component::Light>();
      light_comp->light.color.ambient = glm::vec3(0.3f);
      light_entity->add_component(light_comp);
    }
    light_entity->rotation = glm::vec3(0.2f, -1.0f, 1.2f);
    realm->add_entity(light_entity);

    // camera input
    const auto camera_service = axgl->get_service<axgl::impl::CameraService>("camera");
    camera_service->set_camera_mode(std::make_shared<axgl::impl::Keyboard3DFreeFlyCameraMode>());
    camera_service->set_camera(camera_entity);

    // gui entity
    const auto gui_element = realm_service->create_entity<axgl::interface::entity::GuiElement>();
    gui_element->props.size = glm::vec2(1, 1);
    gui_element->props.bg_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    gui_element->props.font = "arial,noto-tc";
    gui_element->props.content = reinterpret_cast<const char*>(
      u8"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 !@#$%^&*()[]{}<>,.`~-_+=\\/|?'\":;"
      u8"蒙沙新書章節論；附【優價】電影放映。學校商店：千手藝伎百科全書《長屋齋梶地寺大急平町地區大村》。");
    gui_element->props.font_size = 48;
    gui_element->props.fg_color = glm::vec4(1.0f, 0.5f, 0.2f, 1.0f);
    realm->add_entity(gui_element);
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
  axgl.register_service("app", std::make_shared<Application>());
  axgl.run();
}
