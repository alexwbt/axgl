#pragma once

#include <span>
#include <string_view>

#include <axgl/common.hpp>
#include <axgl/interface/component.hpp>
#include <axgl/interface/material.hpp>

namespace axgl::component
{

class Mesh : virtual public Component
{
public:
  static constexpr std::string_view kTypeId = "component::mesh";

  virtual void set_vertices(const std::span<const glm::vec2>& vertices) = 0;
  virtual void set_vertices(const std::span<const glm::vec3>& vertices) = 0;
  virtual void set_normals(const std::span<const glm::vec3>& normals) = 0;
  virtual void set_uv(const std::span<const glm::vec2>& uv) = 0;
  virtual void set_indices(const std::span<const std::uint32_t>& indices) = 0;

  virtual void set_material(ptr_t<Material> material) = 0;
  [[nodiscard]] virtual ptr_t<Material> get_material() const = 0;
};

} // namespace axgl::component

