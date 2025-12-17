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

  virtual ~Material() = default;
  virtual void set_gloss(float gloss) = 0;
  // default color should be white
  virtual void set_color(const glm::vec4& color) = 0;
  // default cull mode should be kCCW
  virtual void set_cull_mode(axgl::Material::CullMode cull_mode) = 0;
  virtual void set_enable_blend(bool enable_blend) = 0;
  virtual void set_alpha_discard(float alpha_discard) = 0;
  virtual void set_property(const std::string& key, const std::string& value) = 0;
  virtual void add_texture(axgl::Material::TextureType type, axgl::ptr_t<axgl::Texture> texture) = 0;
};

} // namespace axgl
