#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/components/camera.hpp>
#include <axgl/impl/entity_base.hpp>

class CameraEntity : public axgl::impl::EntityBase
{
public:
  static constexpr std::string_view kTypeId = "entity::camera";

private:
  axgl::ptr_t<axgl::impl::component::Camera> camera_;
  axgl::ptr_t<axgl::impl::component::Light> flashlight_;

public:
  void on_create(const axgl::Realm::Context& context) override
  {
    const auto& axgl = context.axgl;
    const auto& entity_service = axgl.entity_service();

    // camera
    camera_ = entity_service->create_component_t<axgl::impl::component::Camera>();
    components().add(camera_);

    // flashlight
    flashlight_ = entity_service->create_component_t<axgl::impl::component::Light>();
    flashlight_->light = axgl::Light(
      glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
      axgl::Light::Color{
        .ambient = glm::vec3(0.01f),
        .diffuse = glm::vec3(1.0f),
        .specular = glm::vec3(1.0f),
      },
      axgl::Light::Strength{
        .constant = 1.0f,
        .linear = 0.09f,
        .quadratic = 0.032f,
      },
      12.5f, 15.0f);
    components().add(flashlight_);
  }

  void update(const axgl::Realm::Context& context) override
  {
    EntityBase::update(context);

    flashlight_->light.direction = camera_->camera.front();
  }
};
