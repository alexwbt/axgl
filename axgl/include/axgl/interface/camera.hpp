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

  [[nodiscard]] glm::vec3 up() const { return up_; }
  [[nodiscard]] glm::vec3 front() const { return front_; }
  [[nodiscard]] glm::vec3 horizontal_right() const { return horizontal_right_; }
  [[nodiscard]] glm::mat4 projection_view_matrix() const { return projection_view_matrix_; }

  void update()
  {
    const auto yaw_radians = glm::radians(yaw);
    const auto pitch_radians = glm::radians(pitch);
    const auto roll_radians = glm::radians(roll);

    const auto sin_yaw = glm::sin(yaw_radians);
    const auto cos_yaw = glm::cos(yaw_radians);
    const auto sin_pitch = glm::sin(pitch_radians);
    const auto cos_pitch = glm::cos(pitch_radians);

    front_ = glm::normalize(glm::vec3(sin_yaw * sin_pitch, cos_pitch, cos_yaw * sin_pitch));

    const auto rotation = glm::rotate(glm::mat4(1.0f), roll_radians, front_);
    horizontal_right_ = glm::normalize(glm::cross(front_, glm::vec3(0, -1, 0)));
    up_ = glm::normalize(glm::vec3(rotation * glm::vec4(glm::cross(front_, horizontal_right_), 1)));

    update_projection_view_matrix();
  }

  void update_projection_view_matrix()
  {
    // view matrix
    const auto view = glm::lookAt(position, position + front_, up_);
    // projection matrix
    glm::mat4 projection;
    if (orthographic)
    {
      const auto v = viewport * 0.5f;
      projection = glm::ortho(-v.x, v.x, -v.y, v.y, near_clip, far_clip);
    }
    else if (viewport.y > 0)
    {
      const float f = glm::radians(fov);
      const float r = viewport.x / viewport.y;
      projection = glm::perspective(f, r, near_clip, far_clip);
    }
    // projection * view
    projection_view_matrix_ = projection * view;
  }

  void set_projection_view_matrix(const glm::mat4& pv)
  {
    projection_view_matrix_ = pv;
  }
};

NAMESPACE_AXGL_INTERFACE_END
