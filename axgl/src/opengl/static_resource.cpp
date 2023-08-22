#include "axgl/opengl/static_resource.h"
#include <spdlog/spdlog.h>

NAMESPACE_OPENGL

std::map<std::string, std::shared_ptr<Texture>> textures;
std::map<uint32_t, std::shared_ptr<ShaderProgram>> shaders;

void load_shader(uint32_t shaderId, std::shared_ptr<ShaderProgram> shader)
{
  shaders.insert({ shaderId, std::move(shader) });
}

std::shared_ptr<ShaderProgram> get_shader(uint32_t shaderId)
{
  auto it = shaders.find(shaderId);
  if (it == shaders.end())
  {
    SPDLOG_ERROR("Failed to get shader: {}", shaderId);
    return nullptr;
  }
  return it->second;
}

std::shared_ptr<Texture> load_texture(const std::string& texture, Texture::Type type)
{
  auto it = textures.find(texture);
  if (it == textures.end())
  {
    auto texture_ptr = Texture::load_2d_texture("res/textures/" + texture, type);
    textures.insert({ texture, texture_ptr });
    return texture_ptr;
  }
  return it->second;
}

std::shared_ptr<Texture> load_skybox(const std::string& texture, Texture::Type type)
{
  auto it = textures.find(texture);
  if (it == textures.end())
  {
    std::vector<std::string> paths{
      "res/textures/" + texture + "/right.png",
      "res/textures/" + texture + "/left.png",
      "res/textures/" + texture + "/top.png",
      "res/textures/" + texture + "/bottom.png",
      "res/textures/" + texture + "/front.png",
      "res/textures/" + texture + "/back.png"
    };
    auto texture_ptr = Texture::load_cubemap_texture(paths, type);
    textures.insert({ texture, texture_ptr });
    return texture_ptr;
  }
  return it->second;
}

NAMESPACE_OPENGL_END
