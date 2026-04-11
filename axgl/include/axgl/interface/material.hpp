#pragma once

#include <string>

#include <axgl/common.hpp>
#include <axgl/interface/texture.hpp>

namespace axgl
{

class Material
{
public:
  enum class TextureType
  {
    kUnknown,
    kDiffuse,
    kSpecular,
    kNormal,
    kHeight,
  };

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

  virtual ~Material() = default;
  virtual void set_color(const glm::vec4& color) = 0;
  virtual void set_line_width(float line_width) = 0;
  virtual void set_draw_mode(axgl::Material::DrawMode draw_mode) = 0;
  virtual void set_cull_mode(axgl::Material::CullMode cull_mode) = 0;
  virtual void set_enable_depth_test(bool enable_depth_test) = 0;
  virtual void set_enable_blend(bool enable_blend) = 0;
  virtual void set_enable_shadow(bool enable_shadow) = 0;
  virtual void set_alpha_discard(float alpha_discard) = 0;
  virtual void set_tiling(glm::vec2 tiling) = 0;
  virtual void set_offset(glm::vec2 offset) = 0;
  virtual void set_depth_scale(float depth_scale) = 0;
  virtual void set_property(const std::string& key, const std::string& value) = 0;
  virtual void add_texture(axgl::Material::TextureType type, axgl::ptr_t<axgl::Texture> texture) = 0;

  [[nodiscard]] virtual glm::vec4 get_color() const = 0;
  [[nodiscard]] virtual float get_line_width() const = 0;
  [[nodiscard]] virtual axgl::Material::DrawMode get_draw_mode() const = 0;
  [[nodiscard]] virtual axgl::Material::CullMode get_cull_mode() const = 0;
  [[nodiscard]] virtual bool get_enable_depth_test() const = 0;
  [[nodiscard]] virtual bool get_enable_blend() const = 0;
  [[nodiscard]] virtual bool get_enable_shadow() const = 0;
  [[nodiscard]] virtual float get_alpha_discard() const = 0;
  [[nodiscard]] virtual glm::vec2 get_tiling() const = 0;
  [[nodiscard]] virtual glm::vec2 get_offset() const = 0;
  [[nodiscard]] virtual float get_depth_scale() const = 0;
  [[nodiscard]] virtual std::string get_property(const std::string& key) const = 0;
  [[nodiscard]] virtual axgl::ptr_t<axgl::Texture> get_texture(axgl::Material::TextureType type) const = 0;
};

} // namespace axgl
