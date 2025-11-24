#pragma once

#include <axgl/axgl.hpp>
#include <axgl/common.hpp>
#include <axgl/impl/component/camera.hpp>
#include <axgl/impl/service.hpp>
#include <axgl/interface/camera.hpp>
#include <axgl/interface/input.hpp>
#include <axgl/interface/service.hpp>

NAMESPACE_AXGL_IMPL

class CameraService : public ServiceBase
{
public:
  class CameraMode
  {
  public:
    virtual ~CameraMode() = default;
    virtual void bind_inputs(std::shared_ptr<interface::InputService> input_service) = 0;
    virtual void unbind_inputs(std::shared_ptr<interface::InputService> input_service) = 0;
    virtual void update(interface::Camera& camera) = 0;
  };

private:
  std::shared_ptr<CameraMode> camera_mode_;
  std::shared_ptr<interface::InputService> input_service_;

  std::shared_ptr<interface::Entity> camera_entity_;
  std::shared_ptr<component::Camera> camera_comp_;

public:
  void set_camera_mode(std::shared_ptr<CameraMode> camera_mode)
  {
    if (camera_mode_)
      camera_mode_->unbind_inputs(input_service_);

    camera_mode_ = std::move(camera_mode);
    camera_mode_->bind_inputs(input_service_);
  }

  void set_camera(std::shared_ptr<interface::Entity> camera_entity)
  {
    camera_entity_ = std::move(camera_entity);
    camera_comp_ = camera_entity_->get_component_t<component::Camera>();
  }

  void initialize() override
  {
    const auto axgl = get_context()->axgl;
    input_service_ = axgl->input_service();
  }

  void update() override
  {
    if (!camera_mode_ || !camera_entity_)
      return;
    camera_mode_->update(camera_comp_->camera);
    camera_entity_->transform()->position = camera_comp_->camera.position;
  }
};

class Keyboard3DFreeFlyCameraMode : public CameraService::CameraMode
{
  std::shared_ptr<interface::InputService> input_service_;
  std::shared_ptr<interface::Input> escape_;
  std::shared_ptr<interface::Pointer> pointer_;
  std::shared_ptr<interface::Input> forward_;
  std::shared_ptr<interface::Input> backward_;
  std::shared_ptr<interface::Input> up_;
  std::shared_ptr<interface::Input> down_;
  std::shared_ptr<interface::Input> left_;
  std::shared_ptr<interface::Input> right_;

  float view_movement_speed_ = 0.2f;
  float camera_speed_ = 0.2f;
  bool controlling_ = true;

public:
  Keyboard3DFreeFlyCameraMode()
  {
    escape_ = std::make_shared<interface::Input>("Toggle Camera Control", interface::InputSource::kKeyEscape);
    pointer_ = std::make_shared<interface::Pointer>("View Movement", interface::PointerSource::kMouseMove);
    forward_ = std::make_shared<interface::Input>("Move Forward", interface::InputSource::kKeyW);
    backward_ = std::make_shared<interface::Input>("Move Backward", interface::InputSource::kKeyS);
    up_ = std::make_shared<interface::Input>("Move Up", interface::InputSource::kKeySpace);
    down_ = std::make_shared<interface::Input>("Move Down", interface::InputSource::kKeyLeftShift);
    left_ = std::make_shared<interface::Input>("Move Left", interface::InputSource::kKeyA);
    right_ = std::make_shared<interface::Input>("Move Right", interface::InputSource::kKeyD);
  }

  void bind_inputs(std::shared_ptr<interface::InputService> input_service) override
  {
    input_service->add_input(escape_);
    input_service->add_pointer(pointer_);
    input_service->add_input(forward_);
    input_service->add_input(backward_);
    input_service->add_input(up_);
    input_service->add_input(down_);
    input_service->add_input(left_);
    input_service->add_input(right_);
    input_service->set_cursor_mode(interface::CursorMode::kLocked);
    input_service_ = std::move(input_service);
  }

  void unbind_inputs(std::shared_ptr<interface::InputService> input_service) override
  {
    input_service->remove_pointer(escape_->id);
    input_service->remove_pointer(pointer_->id);
    input_service->remove_input(forward_->id);
    input_service->remove_input(backward_->id);
    input_service->remove_input(up_->id);
    input_service->remove_input(down_->id);
    input_service->remove_input(left_->id);
    input_service->remove_input(right_->id);
    input_service->set_cursor_mode(axgl::interface::CursorMode::kNormal);
    input_service_ = nullptr;
  }

  void update(interface::Camera& camera) override
  {
    if (escape_->tick == 1 && input_service_)
    {
      controlling_ = !controlling_;

      input_service_->set_cursor_mode(controlling_ ? interface::CursorMode::kLocked : interface::CursorMode::kNormal);
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

class Keyboard2DFreeFlyCameraMode : public CameraService::CameraMode
{
  std::shared_ptr<interface::Input> up_;
  std::shared_ptr<interface::Input> down_;
  std::shared_ptr<interface::Input> left_;
  std::shared_ptr<interface::Input> right_;

  float camera_speed_ = 2.0f;

public:
  Keyboard2DFreeFlyCameraMode()
  {
    up_ = std::make_shared<interface::Input>("Move Up", interface::InputSource::kKeyW);
    down_ = std::make_shared<interface::Input>("Move Down", interface::InputSource::kKeyS);
    left_ = std::make_shared<interface::Input>("Move Left", interface::InputSource::kKeyA);
    right_ = std::make_shared<interface::Input>("Move Right", interface::InputSource::kKeyD);
  }

  void bind_inputs(std::shared_ptr<interface::InputService> input_service) override
  {
    input_service->add_input(up_);
    input_service->add_input(down_);
    input_service->add_input(left_);
    input_service->add_input(right_);
  }

  void unbind_inputs(std::shared_ptr<interface::InputService> input_service) override
  {
    input_service->remove_input(up_->id);
    input_service->remove_input(down_->id);
    input_service->remove_input(left_->id);
    input_service->remove_input(right_->id);
  }

  void update(interface::Camera& camera) override
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

NAMESPACE_AXGL_IMPL_END

NAMESPACE_AXGL

template <> inline std::shared_ptr<impl::CameraService> Axgl::use_service()
{
  auto camera_service = std::make_shared<impl::CameraService>();
  register_service("camera", camera_service);

  return camera_service;
}

NAMESPACE_AXGL_END
