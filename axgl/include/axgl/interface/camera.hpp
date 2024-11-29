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
  bool orthographic = false;

private:
  glm::vec3 up_{ 0.0f };
  glm::vec3 right_{ 0.0f };
  glm::vec3 front_{ 0.0f };
  glm::vec3 front_side_{ 0.0f };
  glm::mat4 view_matrix_{ 0.0f };

public:
  Camera() { update(); }

  glm::vec3 up() const { return up_; }
  glm::vec3 right() const { return right_; }
  glm::vec3 front() const { return front_; }
  glm::vec3 front_side() const { return front_side_; }
  glm::mat4 view_matrix() const { return view_matrix_; }

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

  glm::mat4 pv(glm::vec2 viewport) const
  {
    return orthographic ? orthographic_pv(viewport) : perspective_pv(viewport);
  }

private:
  glm::mat4 perspective_pv(glm::vec2 viewport) const
  {
    float f = glm::radians(fov);
    float r = viewport.x / viewport.y;
    glm::mat4 projection = glm::perspective(f, r, near_clip, far_clip);
    return projection * view_matrix();
  }

  glm::mat4 orthographic_pv(glm::vec2 viewport) const
  {
    glm::vec2 v = viewport * 0.5f;
    glm::mat4 projection = glm::ortho(-v.x, v.x, v.y, -v.y, near_clip, far_clip);
    return projection * view_matrix();
  }
};

NAMESPACE_AXGL_INTERFACE_END
