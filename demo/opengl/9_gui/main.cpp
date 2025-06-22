#include <random>

#include <axgl/axgl.hpp>
#include <axgl/impl/glfw.hpp>
#include <axgl/impl/opengl.hpp>
#include <axgl/impl/realm_service.hpp>
#include <axgl/impl/resource_service.hpp>
#include <axgl/impl/camera_service.hpp>

#include <demo_opengl_gui/res.hpp>

#include "base.hpp"
#include "cube.hpp"
#include "grass.hpp"

class Application final : public Base
{
public:
  Application(std::string window_title) : Base(std::move(window_title)) {}

  void initialize() override
  {
    Base::initialize();

    const auto axgl = get_context()->axgl;
    const auto realm_service = axgl->realm_service();
    const auto realm = realm_service->get_active_realm();

    // resources
    const auto resource_service = axgl->resource_service();
    resource_service->load_resources(demo_opengl_gui_res::data);

    // cube entity
    const auto cube = create_cube(axgl);
    realm->add_entity(cube);

    // random generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution pos_dis(-10.0f, 10.0f);
    std::uniform_real_distribution rot_dis(0.0f, 3.1415f);
    // grass entities
    for (int i = 0; i < 1000; i++)
    {
      const auto grass = create_grass(axgl);
      grass->transform()->position.x = pos_dis(gen);
      grass->transform()->position.y = -2;
      grass->transform()->position.z = pos_dis(gen);
      grass->transform()->rotation.y = rot_dis(gen);
      grass->update_model_matrix();
      realm->add_entity(grass);
    }

    // gui element
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
  axgl.register_service("app", std::make_shared<Application>("Hello Gui!"));
  axgl.run();
}
