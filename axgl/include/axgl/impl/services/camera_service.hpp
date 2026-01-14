#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/camera.hpp>
#include <axgl/interface/camera_mode.hpp>
#include <axgl/interface/services/camera_service.hpp>
#include <axgl/interface/services/input_service.hpp>

#include <axgl/impl/components/camera.hpp>

namespace axgl::impl
{

class CameraService : virtual public axgl::CameraService
{
  axgl::ptr_t<axgl::CameraMode> camera_mode_;
  axgl::ptr_t<axgl::InputService> input_service_;

  axgl::ptr_t<axgl::Entity> camera_entity_;
  axgl::ptr_t<axgl::impl::component::Camera> camera_comp_;

public:
  void set_camera_mode(axgl::ptr_t<axgl::CameraMode> camera_mode) override
  {
    if (camera_mode_) camera_mode_->unbind_inputs(input_service_);

    camera_mode_ = std::move(camera_mode);
    camera_mode_->bind_inputs(input_service_);
  }

  void set_camera(axgl::ptr_t<axgl::Entity> camera_entity) override { camera_entity_ = std::move(camera_entity); }

  axgl::Camera* get_camera() override { return camera_comp_ ? &camera_comp_->camera : nullptr; }

  axgl::ptr_t<axgl::Entity> get_camera_entity() override { return camera_entity_; }

  void initialize(const Service::Context& context) override { input_service_ = context.axgl.input_service(); }

  void update(const Service::Context& context) override
  {
    if (!camera_entity_) return;

    camera_comp_ = camera_entity_->components().get_t<component::Camera>();
    if (!camera_comp_) return;

    auto& transform = camera_entity_->transform();
    camera_comp_->camera.position = transform.position;

    if (!camera_mode_) return;
    camera_mode_->update(camera_comp_->camera);
    transform.position = camera_comp_->camera.position;
    camera_entity_->update_model_matrix();
  }
};

} // namespace axgl::impl
