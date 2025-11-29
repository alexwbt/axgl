#pragma once

#include <vector>

#include <glm/glm.hpp>

static std::vector cube_vertices = {
  glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(00.5f, 0.5f, -0.5f),  glm::vec3(00.5f, -0.5f, -0.5f),
  glm::vec3(00.5f, 0.5f, -0.5f),  glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-0.5f, 0.5f, -0.5f),
  glm::vec3(-0.5f, -0.5f, 0.5f),  glm::vec3(00.5f, -0.5f, 0.5f),  glm::vec3(00.5f, 0.5f, 0.5f),
  glm::vec3(00.5f, 0.5f, 0.5f),   glm::vec3(-0.5f, 0.5f, 0.5f),   glm::vec3(-0.5f, -0.5f, 0.5f),
  glm::vec3(-0.5f, 0.5f, 0.5f),   glm::vec3(-0.5f, 0.5f, -0.5f),  glm::vec3(-0.5f, -0.5f, -0.5f),
  glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-0.5f, -0.5f, 0.5f),  glm::vec3(-0.5f, 0.5f, 0.5f),
  glm::vec3(00.5f, 0.5f, 0.5f),   glm::vec3(00.5f, -0.5f, -0.5f), glm::vec3(00.5f, 0.5f, -0.5f),
  glm::vec3(00.5f, -0.5f, -0.5f), glm::vec3(00.5f, 0.5f, 0.5f),   glm::vec3(00.5f, -0.5f, 0.5f),
  glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(00.5f, -0.5f, -0.5f), glm::vec3(00.5f, -0.5f, 0.5f),
  glm::vec3(00.5f, -0.5f, 0.5f),  glm::vec3(-0.5f, -0.5f, 0.5f),  glm::vec3(-0.5f, -0.5f, -0.5f),
  glm::vec3(-0.5f, 0.5f, -0.5f),  glm::vec3(00.5f, 0.5f, 0.5f),   glm::vec3(00.5f, 0.5f, -0.5f),
  glm::vec3(00.5f, 0.5f, 0.5f),   glm::vec3(-0.5f, 0.5f, -0.5f),  glm::vec3(-0.5f, 0.5f, 0.5f),
};

static std::vector cube_normals = {
  glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f),
  glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f),
  glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec3(0.0f, 0.0f, 1.0f),
  glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec3(0.0f, 0.0f, 1.0f),
  -glm::vec3(1.0f, 0.0f, 0.0f), -glm::vec3(1.0f, 0.0f, 0.0f), -glm::vec3(1.0f, 0.0f, 0.0f),
  -glm::vec3(1.0f, 0.0f, 0.0f), -glm::vec3(1.0f, 0.0f, 0.0f), -glm::vec3(1.0f, 0.0f, 0.0f),
  glm::vec3(1.0f, 0.0f, 0.0f),  glm::vec3(1.0f, 0.0f, 0.0f),  glm::vec3(1.0f, 0.0f, 0.0f),
  glm::vec3(1.0f, 0.0f, 0.0f),  glm::vec3(1.0f, 0.0f, 0.0f),  glm::vec3(1.0f, 0.0f, 0.0f),
  glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
  glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
  glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec3(0.0f, 1.0f, 0.0f),
  glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec3(0.0f, 1.0f, 0.0f),
};
