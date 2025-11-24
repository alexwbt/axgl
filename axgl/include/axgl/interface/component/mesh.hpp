#pragma once

#include <span>

#include <axgl/common.hpp>
#include <axgl/interface/realm.hpp>
#include <axgl/interface/renderer.hpp>

#include <glm/glm.hpp>

namespace axgl
{
namespace interface
{

namespace component
{

class Mesh : virtual public Component
{
public:
  virtual void set_vertices(const std::span<const glm::vec2>& vertices) = 0;
  virtual void set_vertices(const std::span<const glm::vec3>& vertices) = 0;
  virtual void set_normals(const std::span<const glm::vec3>& normals) = 0;
  virtual void set_uv(const std::span<const glm::vec2>& uv) = 0;
  virtual void set_indices(const std::span<const uint32_t>& indices) = 0;

  virtual void set_material(std::shared_ptr<Material> material) = 0;
  virtual std::shared_ptr<Material> get_material() const = 0;
};

} // namespace component

} // namespace interface
} // namespace axgl
