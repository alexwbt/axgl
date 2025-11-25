#pragma once

#include <memory>

#include <axgl/interface/camera.hpp>
#include <axgl/interface/camera_mode.hpp>
#include <axgl/interface/input.hpp>
#include <axgl/interface/services/input_service.hpp>

namespace axgl::impl::camera
{

class Keyboard2DFreeFlyCameraMode : public CameraMode
{
  std::shared_ptr<Input> up_;
  std::shared_ptr<Input> down_;
  std::shared_ptr<Input> left_;
  std::shared_ptr<Input> right_;

  float camera_speed_ = 2.0f;

public:
  Keyboard2DFreeFlyCameraMode()
  {
    up_ = std::make_shared<Input>("Move Up", Input::Source::kKeyW);
    down_ = std::make_shared<Input>("Move Down", Input::Source::kKeyS);
    left_ = std::make_shared<Input>("Move Left", Input::Source::kKeyA);
    right_ = std::make_shared<Input>("Move Right", Input::Source::kKeyD);
  }

  void bind_inputs(std::shared_ptr<InputService> input_service) override
  {
    input_service->add_input(up_);
    input_service->add_input(down_);
    input_service->add_input(left_);
    input_service->add_input(right_);
  }

  void unbind_inputs(std::shared_ptr<InputService> input_service) override
  {
    input_service->remove_input(up_->id);
    input_service->remove_input(down_->id);
    input_service->remove_input(left_->id);
    input_service->remove_input(right_->id);
  }

  void update(Camera& camera) override
  {
    if (up_->tick > 0)
      camera.position += glm::vec3(0, 1, 0) * camera_speed_;
    if (down_->tick > 0)
      camera.position -= glm::vec3(0, 1, 0) * camera_speed_;
    if (right_->tick > 0)
      camera.position += glm::vec3(1, 0, 0) * camera_speed_;
    if (left_->tick > 0)
      camera.position -= glm::vec3(1, 0, 0) * camera_speed_;

    camera.update();
  }
};

} // namespace axgl::impl::camera
