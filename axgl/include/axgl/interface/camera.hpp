#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <axgl/common.hpp>

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
  glm::vec2 viewport{ 0.0f };
  glm::vec3 position{ 0.0f };
  bool orthographic = false;

private:
  glm::vec3 up_{ 0.0f };
  glm::vec3 front_{ 0.0f };
  glm::vec3 horizontal_right_{ 0.0f };
  glm::mat4 projection_view_matrix_{ 0.0f };

public:
  Camera() { update(); }

  glm::vec3 up() const { return up_; }
  glm::vec3 front() const { return front_; }
  glm::vec3 horizontal_right() const { return horizontal_right_; }
  glm::mat4 projection_view_matrix() const { return projection_view_matrix_; }

  void update()
  {
    auto yaw_radians = glm::radians(yaw);
    auto pitch_radians = glm::radians(pitch);
    auto roll_radians = glm::radians(roll);

    auto sin_yaw = glm::sin(yaw_radians);
    auto cos_yaw = glm::cos(yaw_radians);
    auto sin_pitch = glm::sin(pitch_radians);
    auto cos_pitch = glm::cos(pitch_radians);

    front_ = glm::normalize(glm::vec3(sin_yaw * sin_pitch, cos_pitch, cos_yaw * sin_pitch));

    auto rotation = glm::rotate(glm::mat4(1.0f), roll_radians, front_);
    horizontal_right_ = glm::normalize(glm::cross(front_, glm::vec3(0, -1, 0)));
    up_ = glm::normalize(glm::vec3(rotation * glm::vec4(glm::cross(front_, horizontal_right_), 1)));

    update_projection_view_matrix();
  }

  void update_projection_view_matrix()
  {
    // view matrix
    glm::mat4 view = glm::lookAt(position, position + front_, up_);
    // projection matrix
    glm::mat4 projection;
    if (orthographic)
    {
      glm::vec2 v = viewport * 0.5f;
      projection = glm::ortho(-v.x, v.x, -v.y, v.y, near_clip, far_clip);
    }
    else if (viewport.y > 0)
    {
      float f = glm::radians(fov);
      float r = viewport.x / viewport.y;
      projection = glm::perspective(f, r, near_clip, far_clip);
    }
    // projection * view
    projection_view_matrix_ = projection * view;
  }
};

NAMESPACE_AXGL_INTERFACE_END
