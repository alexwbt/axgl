#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/camera.hpp>
#include <axgl/interface/camera_mode.hpp>
#include <axgl/interface/input.hpp>
#include <axgl/interface/services/input_service.hpp>

namespace axgl::impl::camera
{

class Keyboard2DFreeFlyCameraMode : public axgl::CameraMode
{
  axgl::ptr_t<axgl::Input> up_;
  axgl::ptr_t<axgl::Input> down_;
  axgl::ptr_t<axgl::Input> left_;
  axgl::ptr_t<axgl::Input> right_;

  float movement_speed_;

public:
  explicit Keyboard2DFreeFlyCameraMode(const float movement_speed = 2.0f) : movement_speed_(movement_speed)
  {
    up_ = axgl::create_ptr<axgl::Input>("Move Up", axgl::Input::Source::kKeyW);
    down_ = axgl::create_ptr<axgl::Input>("Move Down", axgl::Input::Source::kKeyS);
    left_ = axgl::create_ptr<axgl::Input>("Move Left", axgl::Input::Source::kKeyA);
    right_ = axgl::create_ptr<axgl::Input>("Move Right", axgl::Input::Source::kKeyD);
  }

  void bind_inputs(const axgl::ptr_t<axgl::InputService> input_service) override
  {
    input_service->add_input(up_);
    input_service->add_input(down_);
    input_service->add_input(left_);
    input_service->add_input(right_);
  }

  void unbind_inputs(const axgl::ptr_t<axgl::InputService> input_service) override
  {
    input_service->remove_input(up_->id);
    input_service->remove_input(down_->id);
    input_service->remove_input(left_->id);
    input_service->remove_input(right_->id);
  }

  void update(axgl::Camera& camera) override
  {
    if (up_->tick > 0)
      camera.position += glm::vec3(0, 1, 0) * movement_speed_;
    if (down_->tick > 0)
      camera.position -= glm::vec3(0, 1, 0) * movement_speed_;
    if (right_->tick > 0)
      camera.position += glm::vec3(1, 0, 0) * movement_speed_;
    if (left_->tick > 0)
      camera.position -= glm::vec3(1, 0, 0) * movement_speed_;

    camera.update();
  }
};

} // namespace axgl::impl::camera
