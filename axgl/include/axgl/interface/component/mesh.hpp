#pragma once

#include <vector>

#include "axgl/namespace.hpp"
#include "axgl/interface/component/component.hpp"

NAMESPACE_AXGL_INTERFACE

struct Vertex2D
{
  float x;
  float y;
};

class Mesh2D : public Component
{
public:
  virtual void set_data(const std::vector<Vertex2D>& data) = 0;
  virtual void set_indices(const std::vector<uint32_t>& indices) = 0;
};

struct Vertex3D
{
  float x;
  float y;
  float z;
};

class Mesh3D : public Component
{
public:
  virtual void set_data(const std::vector<Vertex3D>& data) = 0;
  virtual void set_indices(const std::vector<uint32_t>& indices) = 0;
};

NAMESPACE_AXGL_INTERFACE_END
