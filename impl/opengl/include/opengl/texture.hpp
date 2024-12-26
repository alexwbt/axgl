#pragma once

#include <span>
#include <array>
#include <string>
#include <stdint.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad/glad.h>
#include <spdlog/spdlog.h>

namespace opengl
{

  class StbiImage final
  {
  public:
    stbi_uc* stbi_ptr;
    GLenum format;
    int width, height;

    StbiImage(const std::span<const uint8_t>& data)
    {
      int nrChannels;
      stbi_ptr = stbi_load_from_memory(data.data(), data.size(), &width, &height, &nrChannels, 0);

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

    ~StbiImage()
    {
      stbi_image_free(stbi_ptr);
    }
  };

  class Texture final
  {
  private:
    GLuint id_ = 0;
    GLuint target_ = 0;

  public:
    Texture()
    {
      glGenTextures(1, &id_);
    }

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other)
    {
      id_ = other.id_;
      other.id_ = 0;
      target_ = other.target_;
      other.target_ = 0;
    }
    Texture& operator=(Texture&& other)
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

    GLuint get_id() const
    {
      return id_;
    }

    void use() const
    {
      glBindTexture(target_, id_);
    }

    void set_parameteri(GLenum param, GLint value) const
    {
      glTexParameteri(target_, param, value);
    }

    void generate_mipmap() const
    {
      glGenerateMipmap(target_);
    }

    void load_texture(
      GLint level,
      GLint internalformat,
      GLsizei width,
      GLsizei height,
      GLint border,
      GLenum format,
      GLenum type,
      const void* pixels)
    {
      if (target_ > 0)
      {
        SPDLOG_ERROR("Texture is already loaded.");
        return;
      }
      target_ = GL_TEXTURE_2D;

      use();
      glTexImage2D(target_, level,
        internalformat, width, height,
        border, format, type, pixels);
    }

    void load_image_texture(std::span<const uint8_t> data)
    {
      StbiImage texture(data);
      if (!texture.stbi_ptr)
      {
        SPDLOG_ERROR("Failed to load texture image.");
        return;
      }

      load_texture(
        0, texture.format,
        texture.width, texture.height,
        0, texture.format, GL_UNSIGNED_BYTE, texture.stbi_ptr);
    }

    void load_cubemap_texture(const std::array<std::span<const uint8_t>, 6>& data)
    {
      if (target_ > 0)
      {
        SPDLOG_ERROR("Texture is already loaded.");
        return;
      }
      target_ = GL_TEXTURE_CUBE_MAP;

      StbiImage texture[6] = {
        data[0], data[1], data[2],
        data[3], data[4], data[5],
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
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
          texture[i].format, texture[i].width, texture[i].height,
          0, texture[i].format, GL_UNSIGNED_BYTE, texture[i].stbi_ptr);
    }
  };

}
