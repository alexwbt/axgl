#pragma once

#include <string>
#include <vector>
#include <memory>

#include "glad/glad.h"
#include "axgl/namespace.h"
#include "axgl/opengl/shader_program.h"

NAMESPACE_OPENGL

class Texture final
{
public:
  enum Type
  {
    kDiffuse,
    kSpecular,
    kNormal,
    kHeight
  };

  static std::shared_ptr<Texture> load_2d_texture(const std::string& path, Type type = kDiffuse);
  static std::shared_ptr<Texture> load_cubemap_texture(std::vector<std::string>& paths, Type type = kDiffuse);

private:
  uint32_t id_;
  Type type_;
  GLenum gl_texture_type_;

public:
  Texture(uint32_t id, Type type, GLenum gl_texture_type);
  ~Texture();

  void use(
    std::shared_ptr<ShaderProgram> shader,
    int i, const std::string& name);

  Type texture_type();
};

NAMESPACE_OPENGL_END
