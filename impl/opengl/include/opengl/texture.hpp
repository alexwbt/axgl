#pragma once

#include <array>
#include <cstdint>
#include <span>

#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <stb_image.h>

namespace opengl
{

class StbiImage final
{
public:
  stbi_uc* stbi_ptr;
  GLenum format;
  int width = 0, height = 0;

  explicit StbiImage(const std::span<const uint8_t>& data)
  {
    int nrChannels;
    stbi_ptr = stbi_load_from_memory(data.data(), static_cast<int>(data.size()), &width, &height, &nrChannels, 0);

    switch (nrChannels)
    {
    case 1: format = GL_RED; break;
    case 4: format = GL_RGBA; break;
    default: format = GL_RGB;
    }
  }
  StbiImage(const StbiImage&) = delete;
  StbiImage& operator=(const StbiImage&) = delete;
  StbiImage(StbiImage&&) = delete;
  StbiImage& operator=(StbiImage&&) = delete;

  ~StbiImage() { stbi_image_free(stbi_ptr); }
};

class Texture final
{
  GLuint id_ = 0;
  GLuint target_ = 0;
  GLsizei width_ = 0;
  GLsizei height_ = 0;

public:
  Texture() { glGenTextures(1, &id_); }

  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;

  Texture(Texture&& other) noexcept
  {
    id_ = other.id_;
    other.id_ = 0;
    target_ = other.target_;
    other.target_ = 0;
  }
  Texture& operator=(Texture&& other) noexcept
  {
    if (this != &other)
    {
      if (id_ > 0)
        glDeleteTextures(1, &id_);

      id_ = other.id_;
      other.id_ = 0;
      target_ = other.target_;
      other.target_ = 0;
    }
    return *this;
  }

  ~Texture()
  {
    if (id_ > 0)
      glDeleteTextures(1, &id_);
  }

  [[nodiscard]] GLuint get_id() const { return id_; }

  [[nodiscard]] GLsizei get_width() const { return width_; }

  [[nodiscard]] GLsizei get_height() const { return height_; }

  void use() const { glBindTexture(target_, id_); }

  void set_parameteri(const GLenum param, const GLint value) const { glTexParameteri(target_, param, value); }

  void generate_mipmap() const { glGenerateMipmap(target_); }

  void load_texture(
    const GLint level,
    const GLint internalformat,
    const GLsizei width,
    const GLsizei height,
    const GLint border,
    const GLenum format,
    const GLenum type,
    const void* pixels)
  {
    if (target_ > 0)
    {
      SPDLOG_ERROR("Texture is already loaded.");
      return;
    }
    target_ = GL_TEXTURE_2D;

    width_ = width;
    height_ = height;

    use();
    glTexImage2D(target_, level, internalformat, width, height, border, format, type, pixels);
  }

  void load_image_texture(const std::span<const uint8_t> data)
  {
    const StbiImage texture(data);
    if (!texture.stbi_ptr)
    {
      SPDLOG_ERROR("Failed to load texture image.");
      return;
    }

    load_texture(
      0, texture.format, texture.width, texture.height, 0, texture.format, GL_UNSIGNED_BYTE, texture.stbi_ptr);
  }

  void load_cubemap_texture(const std::array<std::span<const uint8_t>, 6>& data)
  {
    if (target_ > 0)
    {
      SPDLOG_ERROR("Texture is already loaded.");
      return;
    }
    target_ = GL_TEXTURE_CUBE_MAP;

    const StbiImage texture[6] = {
      StbiImage(data[0]), StbiImage(data[1]), StbiImage(data[2]),
      StbiImage(data[3]), StbiImage(data[4]), StbiImage(data[5]),
    };
    for (int i = 0; i < 6; i++)
    {
      if (!texture[i].stbi_ptr)
      {
        SPDLOG_ERROR("Failed to load cubemap texture. ({})", i);
        return;
      }
    }

    use();
    set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    set_parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    set_parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    set_parameteri(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (int i = 0; i < 6; i++)
      glTexImage2D(
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, texture[i].format, texture[i].width, texture[i].height, 0,
        texture[i].format, GL_UNSIGNED_BYTE, texture[i].stbi_ptr);
  }
};

} // namespace opengl
