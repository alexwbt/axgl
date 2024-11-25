#pragma once

#include <vector>

#include "axgl/namespace.hpp"
#include "axgl/interface/realm.hpp"

#include <glm/glm.hpp>

NAMESPACE_AXGL_INTERFACE

class Mesh2D : public Component
{
public:
  virtual void set_vertices(const std::vector<glm::vec2>& vertices) = 0;
  virtual void set_indices(const std::vector<uint32_t>& indices) = 0;
  virtual void set_color(const glm::vec3& color) = 0;
};

class Mesh3D : public Component
{
public:
  virtual void set_vertices(const std::vector<glm::vec3>& vertices) = 0;
  virtual void set_normals(const std::vector<glm::vec3>& normals) = 0;
  virtual void set_indices(const std::vector<uint32_t>& indices) = 0;
  virtual void set_color(const glm::vec3& color) = 0;
};

NAMESPACE_AXGL_INTERFACE_END
