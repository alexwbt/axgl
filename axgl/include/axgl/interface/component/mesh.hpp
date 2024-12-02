#pragma once

#include <span>

#include "axgl/namespace.hpp"
#include "axgl/interface/realm.hpp"
#include "axgl/interface/renderer.hpp"

#include <glm/glm.hpp>

NAMESPACE_AXGL_INTERFACE

class Mesh2D : public Component
{
public:
  virtual void set_vertices(const std::span<const glm::vec2>& vertices) = 0;
  virtual void set_uv(const std::span<const glm::vec2>& uv) = 0;

  virtual void set_indices(const std::span<const uint32_t>& indices) = 0;
  virtual void set_color(const glm::vec3& color) = 0;

  virtual void add_texture(Texture::Type type, std::shared_ptr<Texture> texture) = 0;
};

class Mesh3D : public Component
{
public:
  virtual void set_vertices(const std::span<const glm::vec3>& vertices) = 0;
  virtual void set_normals(const std::span<const glm::vec3>& normals) = 0;
  virtual void set_uv(const std::span<const glm::vec2>& uv) = 0;

  virtual void set_indices(const std::span<const uint32_t>& indices) = 0;
  virtual void set_color(const glm::vec3& color) = 0;

  virtual void add_texture(Texture::Type type, std::shared_ptr<Texture> texture) = 0;
};

NAMESPACE_AXGL_INTERFACE_END
