#pragma once

#include "axgl/common.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

NAMESPACE_AXGL_INTERFACE

class Camera final
{
public:
  float fov = 80.0f;
  float yaw = 0.0f;
  float pitch = 90.0f;
  float roll = 0.0f;
  float near_clip = 0.1f;
  float far_clip = 1000.0f;
  glm::vec3 position{ 0.0f };
  bool orthographic = false;

private:
  glm::vec3 up_{ 0.0f };
  glm::vec3 front_{ 0.0f };
  glm::vec3 horizontal_right_{ 0.0f };
  glm::mat4 view_matrix_{ 0.0f };

public:
  Camera() { update(); }

  glm::vec3 up() const { return up_; }
  glm::vec3 front() const { return front_; }
  glm::vec3 horizontal_right() const { return horizontal_right_; }
  glm::mat4 view_matrix() const { return view_matrix_; }

  void update()
  {
    auto yaw_radians = glm::radians(yaw);
    auto pitch_radians = glm::radians(pitch);
    auto roll_radians = -glm::radians(roll);

    auto sin_yaw = glm::sin(yaw_radians);
    auto cos_yaw = glm::cos(yaw_radians);
    auto sin_pitch = glm::sin(pitch_radians);
    auto cos_pitch = glm::cos(pitch_radians);

    front_ = glm::normalize(glm::vec3(sin_yaw * sin_pitch, cos_pitch, cos_yaw * sin_pitch));

    auto rotation = glm::rotate(glm::mat4(1.0f), roll_radians, front_);
    horizontal_right_ = glm::normalize(glm::cross(front_, glm::vec3(0, 1, 0)));
    up_ = glm::normalize(glm::vec3(rotation * glm::vec4(glm::cross(horizontal_right_, front_), 1)));

    view_matrix_ = glm::lookAt(position, position + front_, up_);
  }

  glm::mat4 pv(const glm::vec2& viewport) const
  {
    return orthographic ? orthographic_pv(viewport) : perspective_pv(viewport);
  }

private:
  glm::mat4 perspective_pv(const glm::vec2& viewport) const
  {
    float f = glm::radians(fov);
    float r = viewport.x / viewport.y;
    glm::mat4 projection = glm::perspective(f, r, near_clip, far_clip);
    return projection * view_matrix();
  }

  glm::mat4 orthographic_pv(const glm::vec2& viewport) const
  {
    glm::vec2 v = viewport * 0.5f;
    glm::mat4 projection = glm::ortho(v.x, -v.x, -v.y, v.y, near_clip, far_clip);
    return projection * view_matrix();
  }
};

NAMESPACE_AXGL_INTERFACE_END
