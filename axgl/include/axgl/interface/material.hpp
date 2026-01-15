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
  virtual void set_alpha_discard(float alpha_discard) = 0;
  virtual void set_tiling(glm::vec2 tiling) = 0;
  virtual void set_offset(glm::vec2 offset) = 0;
  virtual void set_property(const std::string& key, const std::string& value) = 0;
  virtual void add_texture(axgl::Material::TextureType type, axgl::ptr_t<axgl::Texture> texture) = 0;
};

} // namespace axgl
