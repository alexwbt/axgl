#pragma once

#include <memory>
#include <string>

#include <glm/glm.hpp>

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
  // default color should be white
  virtual void set_color(const glm::vec4& color) = 0;
  // default cull mode should be kCCW
  virtual void set_cull_mode(CullMode cull_mode) = 0;
  virtual void set_enable_blend(bool enable_blend) = 0;
  virtual void set_prop(const std::string& key, const std::string& value) = 0;
  virtual void add_texture(TextureType type, std::shared_ptr<Texture> texture) = 0;
};

} // namespace axgl
