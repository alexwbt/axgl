#include "axgl/opengl/static_resource.h"

NAMESPACE_OPENGL

std::map<uint32_t, std::shared_ptr<Texture>> textures;
std::map<uint32_t, std::shared_ptr<ShaderProgram>> shaders;

void load_texture(uint32_t textureId, std::shared_ptr<Texture> texture)
{
  textures.insert({ textureId, std::move(texture) });
}

std::shared_ptr<Texture> get_texture(uint32_t textureId)
{
  return textures.find(textureId)->second;
}

void load_shader(uint32_t shaderId, std::shared_ptr<ShaderProgram> shader)
{
  shaders.insert({ shaderId, std::move(shader) });
}

std::shared_ptr<ShaderProgram> get_shader(uint32_t shaderId)
{
  return shaders.find(shaderId)->second;
}

NAMESPACE_OPENGL_END
