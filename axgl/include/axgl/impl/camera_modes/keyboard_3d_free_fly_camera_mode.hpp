#pragma once

#include <memory>

#include <glm/glm.hpp>

#include <axgl/interface/camera_mode.hpp>
#include <axgl/interface/input.hpp>
#include <axgl/interface/pointer.hpp>
#include <axgl/interface/services/input_service.hpp>

namespace axgl::impl::camera_modes
{

class Keyboard3DFreeFlyCameraMode : public CameraMode
{
  std::shared_ptr<InputService> input_service_;
  std::shared_ptr<Input> escape_;
  std::shared_ptr<Pointer> pointer_;
  std::shared_ptr<Input> forward_;
  std::shared_ptr<Input> backward_;
  std::shared_ptr<Input> up_;
  std::shared_ptr<Input> down_;
  std::shared_ptr<Input> left_;
  std::shared_ptr<Input> right_;

  float view_movement_speed_ = 0.2f;
  float camera_speed_ = 0.2f;
  bool controlling_ = true;

public:
  Keyboard3DFreeFlyCameraMode()
  {
    escape_ = std::make_shared<Input>("Toggle Camera Control", Input::Source::kKeyEscape);
    pointer_ = std::make_shared<Pointer>("View Movement", Pointer::Source::kMouseMove);
    forward_ = std::make_shared<Input>("Move Forward", Input::Source::kKeyW);
    backward_ = std::make_shared<Input>("Move Backward", Input::Source::kKeyS);
    up_ = std::make_shared<Input>("Move Up", Input::Source::kKeySpace);
    down_ = std::make_shared<Input>("Move Down", Input::Source::kKeyLeftShift);
    left_ = std::make_shared<Input>("Move Left", Input::Source::kKeyA);
    right_ = std::make_shared<Input>("Move Right", Input::Source::kKeyD);
  }

  void bind_inputs(std::shared_ptr<InputService> input_service) override
  {
    input_service->add_input(escape_);
    input_service->add_pointer(pointer_);
    input_service->add_input(forward_);
    input_service->add_input(backward_);
    input_service->add_input(up_);
    input_service->add_input(down_);
    input_service->add_input(left_);
    input_service->add_input(right_);
    input_service->set_cursor_mode(InputService::CursorMode::kLocked);
    input_service_ = std::move(input_service);
  }

  void unbind_inputs(std::shared_ptr<InputService> input_service) override
  {
    input_service->remove_pointer(escape_->id);
    input_service->remove_pointer(pointer_->id);
    input_service->remove_input(forward_->id);
    input_service->remove_input(backward_->id);
    input_service->remove_input(up_->id);
    input_service->remove_input(down_->id);
    input_service->remove_input(left_->id);
    input_service->remove_input(right_->id);
    input_service->set_cursor_mode(InputService::CursorMode::kNormal);
    input_service_ = nullptr;
  }

  void update(Camera& camera) override
  {
    if (escape_->tick == 1 && input_service_)
    {
      controlling_ = !controlling_;

      input_service_->set_cursor_mode(
        controlling_ ? InputService::CursorMode::kLocked : InputService::CursorMode::kNormal);
    }

    if (!controlling_)
      return;

    if (pointer_->tick > 1 && pointer_->delta.x != 0)
      camera.yaw += pointer_->delta.x * view_movement_speed_;

    if (pointer_->tick > 1 && pointer_->delta.y != 0)
      camera.pitch = std::min(std::max(camera.pitch + (pointer_->delta.y * view_movement_speed_), 1.0f), 179.0f);

    if (forward_->tick > 0)
      camera.position += camera.front() * camera_speed_;
    if (backward_->tick > 0)
      camera.position -= camera.front() * camera_speed_;
    if (up_->tick > 0)
      camera.position += glm::vec3(0, 1, 0) * camera_speed_;
    if (down_->tick > 0)
      camera.position -= glm::vec3(0, 1, 0) * camera_speed_;
    if (right_->tick > 0)
      camera.position += camera.horizontal_right() * camera_speed_;
    if (left_->tick > 0)
      camera.position -= camera.horizontal_right() * camera_speed_;

    camera.update();
  }
};

} // namespace axgl::impl::camera_modes
