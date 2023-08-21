#pragma once

#include <memory>
#include <array>
#include <glm/glm.hpp>

#include "axgl/namespace.h"

NAMESPACE_WORLD

class Camera
{
public:
  float yaw = 0.0f;
  float pitch = 0.0f;
  float fov = 80.0f;
  glm::vec3 position{ 0.0f };
  float near_clip = 0.1f;
  float far_clip = 100.0f;

private:
  glm::vec3 front_{ 0.0f };
  glm::vec3 front_side_{ 0.0f };
  glm::vec3 right_{ 0.0f };
  glm::vec3 up_{ 0.0f };

  glm::mat4 view_matrix_{ 0.0f };

public:
  Camera();
  virtual ~Camera() {}

  const glm::vec3 front() { return front_; }
  const glm::vec3 front_side() { return front_side_; }
  const glm::vec3 right() { return right_; }
  const glm::vec3 up() { return up_; }

  void update_transform();
  const glm::mat4 view_matrix();
};

class CameraController : public Camera
{
public:
  struct Input
  {
    bool forward, backward, left, right, up, down;
  };

private:
  std::shared_ptr<Camera> camera_;

  float camera_speed_ = 0.1f;

  float pivot_x_ = 0;
  float pivot_y_ = 0;
  float pivot_sensitivity_ = 0.2f;

public:
  void set_camera(std::shared_ptr<Camera> camera);
  void move(const Input& input);
  void pivot(float x, float y);
};

NAMESPACE_WORLD_END
