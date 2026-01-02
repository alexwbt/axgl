#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/camera_mode.hpp>
#include <axgl/interface/input.hpp>
#include <axgl/interface/pointer.hpp>
#include <axgl/interface/services/input_service.hpp>

namespace axgl::impl::camera
{

class Keyboard3DFreeFlyCameraMode : public axgl::CameraMode
{
  axgl::ptr_t<axgl::InputService> input_service_;
  axgl::ptr_t<axgl::Pointer> pointer_;
  axgl::ptr_t<axgl::Input> forward_;
  axgl::ptr_t<axgl::Input> backward_;
  axgl::ptr_t<axgl::Input> up_;
  axgl::ptr_t<axgl::Input> down_;
  axgl::ptr_t<axgl::Input> left_;
  axgl::ptr_t<axgl::Input> right_;
  axgl::ptr_t<axgl::Input> escape_;

  float movement_speed_;
  float view_sensitivity_;
  bool controlling_ = true;

public:
  explicit Keyboard3DFreeFlyCameraMode(
    axgl::ptr_t<Pointer> pointer,
    axgl::ptr_t<Input> forward,
    axgl::ptr_t<Input> backward,
    axgl::ptr_t<Input> up,
    axgl::ptr_t<Input> down,
    axgl::ptr_t<Input> left,
    axgl::ptr_t<Input> right,
    axgl::ptr_t<Input> escape,
    float movement_speed,
    float view_sensitivity) :
    pointer_(std::move(pointer)),
    forward_(std::move(forward)),
    backward_(std::move(backward)),
    up_(std::move(up)),
    down_(std::move(down)),
    left_(std::move(left)),
    right_(std::move(right)),
    escape_(std::move(escape)),
    movement_speed_(movement_speed),
    view_sensitivity_(view_sensitivity)
  {
  }

  explicit Keyboard3DFreeFlyCameraMode(float movement_speed = 0.2f, float view_sensitivity = 0.2f) :
    Keyboard3DFreeFlyCameraMode(
      axgl::create_ptr<Pointer>("View Movement", Pointer::Source::kMouseMove),
      axgl::create_ptr<Input>("Move Forward", Input::Source::kKeyW),
      axgl::create_ptr<Input>("Move Backward", Input::Source::kKeyS),
      axgl::create_ptr<Input>("Move Up", Input::Source::kKeySpace),
      axgl::create_ptr<Input>("Move Down", Input::Source::kKeyLeftShift),
      axgl::create_ptr<Input>("Move Left", Input::Source::kKeyA),
      axgl::create_ptr<Input>("Move Right", Input::Source::kKeyD),
      axgl::create_ptr<Input>("Toggle Camera Control", Input::Source::kKeyEscape),
      movement_speed,
      view_sensitivity)
  {
  }

  void bind_inputs(ptr_t<InputService> input_service) override
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

  void unbind_inputs(ptr_t<InputService> input_service) override
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

    if (!controlling_) return;

    if (pointer_->tick > 1 && pointer_->delta.x != 0.0f) camera.yaw += pointer_->delta.x * view_sensitivity_;

    if (pointer_->tick > 1 && pointer_->delta.y != 0.0f)
      camera.pitch = std::min(std::max(camera.pitch + (pointer_->delta.y * view_sensitivity_), 1.0f), 179.0f);

    if (forward_->tick > 0) camera.position += camera.front() * movement_speed_;
    if (backward_->tick > 0) camera.position -= camera.front() * movement_speed_;
    if (up_->tick > 0) camera.position += glm::vec3(0, 1, 0) * movement_speed_;
    if (down_->tick > 0) camera.position -= glm::vec3(0, 1, 0) * movement_speed_;
    if (right_->tick > 0) camera.position += camera.horizontal_right() * movement_speed_;
    if (left_->tick > 0) camera.position -= camera.horizontal_right() * movement_speed_;

    camera.update();
  }
};

} // namespace axgl::impl::camera
