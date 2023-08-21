#include "axgl/world/camera.h"

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

NAMESPACE_WORLD

Camera::Camera()
{
  update_transform();
}

void Camera::update_transform()
{
  auto yaw_radians = glm::radians(yaw);
  auto pitch_radians = glm::radians(pitch);
  auto cos_yaw_radians = glm::cos(yaw_radians);
  auto sin_yaw_radians = glm::sin(yaw_radians);
  auto cos_pitch_radians = glm::cos(pitch_radians);
  auto sin_pitch_radians = glm::sin(pitch_radians);
  front_ = glm::normalize(glm::vec3(
    cos_yaw_radians * cos_pitch_radians,
    sin_pitch_radians,
    sin_yaw_radians * cos_pitch_radians
  ));
  front_side_ = glm::normalize(glm::vec3(cos_yaw_radians, 0, sin_yaw_radians));
  right_ = glm::normalize(glm::cross(front_, glm::vec3(0, 1, 0)));
  up_ = glm::normalize(glm::cross(right_, front_));

  view_matrix_ = glm::lookAt(position, position + front_, up_);
}

const glm::mat4 Camera::view_matrix()
{
  return view_matrix_;
}

void CameraController::set_camera(std::shared_ptr<Camera> camera)
{
    camera_ = std::move(camera);
}

void CameraController::move(const Input& input)
{
    if (!camera_) return;
    if (input.forward) camera_->position += camera_speed_ * camera_->front();
    if (input.backward) camera_->position -= camera_speed_ * camera_->front();
    if (input.right) camera_->position += camera_speed_ * camera_->right();
    if (input.left) camera_->position -= camera_speed_ * camera_->right();
    if (input.up) camera_->position += camera_speed_ * glm::vec3(0, 1, 0);
    if (input.down) camera_->position -= camera_speed_ * glm::vec3(0, 1, 0);

    camera_->update_transform();
}

void CameraController::pivot(float x, float y)
{
    if (!camera_) return;
    if (pivot_x_ || pivot_y_)
    {
        camera_->yaw += (x - pivot_x_) * pivot_sensitivity_;
        camera_->pitch += (pivot_y_ - y) * pivot_sensitivity_;
    }
    pivot_x_ = x;
    pivot_y_ = y;

    if (camera_->pitch > 89.0f) camera_->pitch = 89.0f;
    if (camera_->pitch < -89.0f) camera_->pitch = -89.0f;

    camera_->update_transform();
}

NAMESPACE_WORLD_END
