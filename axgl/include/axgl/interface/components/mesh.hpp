#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/component.hpp>
#include <axgl/interface/material.hpp>

namespace axgl::component
{

class Mesh : virtual public Component
{
public:
  enum class CullMode
  {
    kNone,
    kCCW,
    kCW,
  };

  enum class DrawMode
  {
    kPoints,
    kLines,
    kLineStrip,
    kLineLoop,
    kTriangles,
    kTriangleStrip,
    kTriangleFan,
  };

  static constexpr std::string_view kTypeId = "component::mesh";

  virtual void set_vertices(const std::span<const glm::vec2>& vertices) = 0;
  virtual void set_vertices(const std::span<const glm::vec3>& vertices) = 0;
  virtual void set_normals(const std::span<const glm::vec3>& normals) = 0;
  virtual void set_tangents(const std::span<const glm::vec3>& tangents) = 0;
  virtual void set_bitangents(const std::span<const glm::vec3>& bitangents) = 0;
  virtual void set_uv(const std::span<const glm::vec2>& uv) = 0;
  virtual void set_indices(const std::span<const std::uint32_t>& indices) = 0;

  virtual void set_line_width(float line_width) = 0;
  virtual void set_draw_mode(axgl::component::Mesh::DrawMode draw_mode) = 0;
  virtual void set_cull_mode(axgl::component::Mesh::CullMode cull_mode) = 0;
  virtual void set_enable_shadow(bool enable_shadow) = 0;

  virtual void set_material(axgl::ptr_t<axgl::Material> material) = 0;
  [[nodiscard]] virtual axgl::ptr_t<axgl::Material> get_material() const = 0;

  [[nodiscard]] virtual float get_line_width() const = 0;
  [[nodiscard]] virtual axgl::component::Mesh::DrawMode get_draw_mode() const
    = 0;
  [[nodiscard]] virtual axgl::component::Mesh::CullMode get_cull_mode() const
    = 0;
  [[nodiscard]] virtual bool get_enable_shadow() const = 0;

  virtual void calculate_tbn() = 0;
};

} // namespace axgl::component
