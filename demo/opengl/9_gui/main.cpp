#include <axgl/axgl.hpp>
#include <axgl/impl/glfw.hpp>
#include <axgl/impl/opengl.hpp>
#include <axgl/impl/realm_service.hpp>
#include <axgl/impl/camera_service.hpp>
#include <axgl/impl/resource_service.hpp>
#include <axgl/impl/component/camera.hpp>
#include <axgl/impl/component/light.hpp>

#include <demo_opengl_gui/res.hpp>

static std::vector<glm::vec3> cube_vertices = {
  glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(00.5f,  0.5f, -0.5f), glm::vec3(00.5f, -0.5f, -0.5f), glm::vec3(00.5f,  0.5f, -0.5f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-0.5f,  0.5f, -0.5f),
  glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(00.5f, -0.5f,  0.5f), glm::vec3(00.5f,  0.5f,  0.5f), glm::vec3(00.5f,  0.5f,  0.5f), glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-0.5f, -0.5f,  0.5f),
  glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-0.5f,  0.5f,  0.5f),
  glm::vec3(00.5f,  0.5f,  0.5f), glm::vec3(00.5f, -0.5f, -0.5f), glm::vec3(00.5f,  0.5f, -0.5f), glm::vec3(00.5f, -0.5f, -0.5f), glm::vec3(00.5f,  0.5f,  0.5f), glm::vec3(00.5f, -0.5f,  0.5f),
  glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(00.5f, -0.5f, -0.5f), glm::vec3(00.5f, -0.5f,  0.5f), glm::vec3(00.5f, -0.5f,  0.5f), glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-0.5f, -0.5f, -0.5f),
  glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(00.5f,  0.5f,  0.5f), glm::vec3(00.5f,  0.5f, -0.5f), glm::vec3(00.5f,  0.5f,  0.5f), glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-0.5f,  0.5f,  0.5f),
};

static std::vector<glm::vec3> cube_normals = {
  glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f,  0.0f, -1.0f),
  glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f,  0.0f,  1.0f),
 -glm::vec3(1.0f,  0.0f,  0.0f),-glm::vec3(1.0f,  0.0f,  0.0f),-glm::vec3(1.0f,  0.0f,  0.0f),-glm::vec3(1.0f,  0.0f,  0.0f),-glm::vec3(1.0f,  0.0f,  0.0f),-glm::vec3(1.0f,  0.0f,  0.0f),
  glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(1.0f,  0.0f,  0.0f),
  glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f),
  glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  1.0f,  0.0f),
};

class Application : public axgl::interface::Service
{
public:
  void initialize() override
  {
    auto axgl = get_context()->axgl;

    // window
    auto window = axgl->window_service()->create_window();
    window->set_title("Hello gui!");

    // input
    axgl->input_service()->set_window(window);

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

    // cube entity
    auto cube = realm_service->create_entity<axgl::interface::Entity>();
    {
      // material
      auto material = renderer_service->create_material("default");
      material->set_color({ 1.0f, 0.5f, 0.2f, 1.0f });

      // cube mesh
      auto mesh = realm_service->create_component<axgl::interface::component::Mesh>();
      mesh->set_vertices(cube_vertices);
      mesh->set_normals(cube_normals);
      mesh->set_material(material);
      cube->add_component(mesh);
    }
    realm->add_entity(cube);

    // camera entity
    auto camera_entity = realm_service->create_entity<axgl::interface::Entity>();
    {
      auto camera_comp = realm_service->create_component<axgl::impl::component::Camera>();
      camera_entity->add_component(camera_comp);
    }
    camera_entity->position.z = -2;
    realm->add_entity(camera_entity);

    // light entity
    auto light_entity = realm_service->create_entity<axgl::interface::Entity>();
    {
      auto light_comp = realm_service->create_component<axgl::impl::component::Light>();
      light_comp->light.color.ambient = glm::vec3(0.3f);
      light_entity->add_component(light_comp);
    }
    light_entity->rotation = glm::vec3(0.2f, -1.0f, 1.2f);
    realm->add_entity(light_entity);

    // camera input
    auto camera_service = axgl->get_service<axgl::impl::CameraService>("camera");
    camera_service->set_camera_mode(std::make_shared<axgl::impl::Keyboard3DFreeFlyCameraMode>());
    camera_service->set_camera(camera_entity);

    // gui entity
    auto gui_element = realm_service->create_entity<axgl::interface::Entity>();
    {
      // gui component
      auto gui_comp = realm_service->create_component<axgl::interface::component::GuiElement>();
      //gui_comp->props.size = window->get_size();
      //gui_comp->props.bg_color = glm::vec4(0.2f, 0.5f, 0.2f, 0.5f);

      gui_comp->props.font = "arial,noto-tc";
      gui_comp->props.content = (const char*)
        u8"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 !@#$%^&*()[]{}<>,.`~-_+=\\/|?'\":;"
        u8"蒙沙新書章節論；附【優價】電影放映。學校商店：千手藝伎百科全書《長屋齋梶地寺大急平町地區大村》。";
      gui_comp->props.font_size = 48;
      gui_comp->props.size = glm::vec2(48, 48);
      gui_comp->props.fg_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
      gui_comp->props.bg_color = glm::vec4(1.0f, 0.5f, 0.2f, 1.0f);

      gui_element->add_component(gui_comp);
    }
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
  axgl.use_service<axgl::impl::OpenglTextService>();
  axgl.register_service("app", std::make_shared<Application>());
  axgl.run();
}
