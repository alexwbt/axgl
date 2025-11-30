#pragma once

#ifdef AXGL_DEBUG
#include <stdexcept>
#endif

#include <axgl/interface/camera.hpp>
#include <axgl/interface/camera_mode.hpp>
#include <axgl/interface/services/camera_service.hpp>
#include <axgl/interface/services/input_service.hpp>

#include <axgl/impl/components/camera.hpp>

namespace axgl::impl
{

class CameraService : virtual public axgl::CameraService
{
  std::shared_ptr<axgl::CameraMode> camera_mode_;
  std::shared_ptr<axgl::InputService> input_service_;

  std::shared_ptr<axgl::Entity> camera_entity_;
  std::shared_ptr<component::Camera> camera_comp_;

public:
  void set_camera_mode(std::shared_ptr<axgl::CameraMode> camera_mode) override
  {
    if (camera_mode_)
      camera_mode_->unbind_inputs(input_service_);

    camera_mode_ = std::move(camera_mode);
    camera_mode_->bind_inputs(input_service_);
  }

  void set_camera(std::shared_ptr<axgl::Entity> camera_entity) override
  {
    camera_entity_ = std::move(camera_entity);
    camera_comp_ = camera_entity_->components()->get_t<component::Camera>();
  }

  axgl::Camera* get_camera() override
  {
#ifdef AXGL_DEBUG
    if (!camera_comp_)
      throw std::runtime_error("Camera not set.");
#endif
    return camera_comp_ ? &camera_comp_->camera : nullptr;
  }

  void initialize(const Service::Context& context) override { input_service_ = context.axgl.input_service(); }

  void update(const Service::Context& context) override
  {
    if (!camera_mode_ || !camera_entity_)
      return;
    camera_mode_->update(camera_comp_->camera);
    camera_entity_->transform().position = camera_comp_->camera.position;
  }
};

} // namespace axgl::impl
