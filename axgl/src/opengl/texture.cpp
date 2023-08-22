#include "axgl/opengl/texture.h"

#include <spdlog/spdlog.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF

NAMESPACE_OPENGL

std::shared_ptr<Texture> Texture::load_2d_texture(const std::string& path, Type type)
{
  uint32_t id;
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  //GLfloat max_aniso = 0.0f;
  //glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
  //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);

  int width, height, nrChannels;
  stbi_uc* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
  if (data)
  {
    GLenum format = GL_RGB;
    if (nrChannels == 1)
      format = GL_RED;
    else if (nrChannels == 4)
      format = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
    SPDLOG_CRITICAL("Failed to load texture: {}", path);

  stbi_image_free(data);

  return std::make_shared<Texture>(id, type, GL_TEXTURE_2D);
}

std::shared_ptr<Texture> Texture::load_cubemap_texture(std::vector<std::string>& paths, Type type)
{
  static constexpr int kSides = 6;
  if (paths.size() != kSides)
  {
    SPDLOG_ERROR("Failed to create cubemap. Invalid paths param.");
    return nullptr;
  }

  uint32_t id;
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_CUBE_MAP, id);

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  int width, height, nrChannels;
  stbi_uc* data;
  for (int i = 0; i < kSides; ++i)
  {
    data = stbi_load(paths[i].c_str(), &width, &height, &nrChannels, 0);
    if (!data)
    {
      SPDLOG_ERROR("Failed to load texture: {}", paths[i]);
      continue;
    }

    GLenum format = GL_RGB;
    if (nrChannels == 1)
      format = GL_RED;
    else if (nrChannels == 4)
      format = GL_RGBA;

    glTexImage2D(
      GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
      0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data
    );
    stbi_image_free(data);
  }

  return std::make_shared<Texture>(id, type, GL_TEXTURE_CUBE_MAP);
}

Texture::Texture(uint32_t id, Type type, GLenum gl_texture_type)
  : id_(id), type_(type), gl_texture_type_(gl_texture_type)
{}

Texture::~Texture()
{
  glDeleteTextures(1, &id_);
}

void Texture::use(
  std::shared_ptr<ShaderProgram> shader,
  int i, const std::string& uniform_name)
{
  if (i >= 32)
  {
    SPDLOG_ERROR("Texture index cannot be more than 31. ({})", i);
    return;
  }

  glActiveTexture(GL_TEXTURE0 + i);
  glBindTexture(gl_texture_type_, id_);

  shader->set_int(uniform_name, i);
}

Texture::Type Texture::texture_type()
{
  return type_;
}

NAMESPACE_OPENGL_END
