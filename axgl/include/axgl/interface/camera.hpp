#pragma once

#include "axgl/namespace.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

NAMESPACE_AXGL_INTERFACE

class Camera final
{
public:
  float fov = 80.0f;
  float yaw = 0.0f;
  float pitch = 0.0f;
  float near_clip = 0.1f;
  float far_clip = 100.0f;
  glm::vec3 position{ 0.0f };

private:
  glm::vec3 up_{ 0.0f };
  glm::vec3 right_{ 0.0f };
  glm::vec3 front_{ 0.0f };
  glm::vec3 front_side_{ 0.0f };
  glm::mat4 view_matrix_{ 0.0f };

public:
  Camera() { update(); }

  const glm::vec3 up() { return up_; }
  const glm::vec3 right() { return right_; }
  const glm::vec3 front() { return front_; }
  const glm::vec3 front_side() { return front_side_; }
  const glm::mat4 view_matrix() { return view_matrix_; }

  void update()
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
};

NAMESPACE_AXGL_INTERFACE_END
