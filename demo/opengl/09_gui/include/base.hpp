#pragma once

#include <../../../../axgl/include/axgl/impl/services/camera_service.hpp>
#include <axgl/axgl.hpp>
#include <axgl/impl/components/camera.hpp>
#include <axgl/impl/components/light.hpp>
#include <axgl/impl/service.hpp>

class Base : public axgl::impl::ServiceBase
{
  std::string window_title_;

public:
  explicit Base(std::string window_title) : window_title_(std::move(window_title)) { }

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

    // realm
    const auto realm_service = axgl->realm_service();
    const auto realm = axgl->realm_service()->create_realm();
    realm->set_renderer(renderer);

    // camera entity
    const auto camera_entity = realm_service->create_entity<axgl::interface::Entity>();
    {
      const auto camera_comp = realm_service->create_component<axgl::impl::component::Camera>();
      camera_entity->add_component(camera_comp);
    }
    camera_entity->transform()->position.z = -2;
    realm->add_entity(camera_entity);

    // light entity
    const auto light_entity = realm_service->create_entity<axgl::interface::Entity>();
    {
      const auto light_comp = realm_service->create_component<axgl::impl::component::Light>();
      light_comp->light.color.ambient = glm::vec3(0.3f);
      light_entity->add_component(light_comp);
    }
    light_entity->transform()->rotation = glm::vec3(0.2f, -1.0f, 1.2f);
    realm->add_entity(light_entity);

    // camera input
    const auto camera_service = axgl->get_service<axgl::impl::CameraService>("camera");
    camera_service->set_camera_mode(std::make_shared<axgl::impl::Keyboard3DFreeFlyCameraMode>());
    camera_service->set_camera(camera_entity);
  }
};
