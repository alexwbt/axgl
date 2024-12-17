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

  class StbiImage
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

    static constexpr int kCubemapSize = 6;

  public:
    Texture()
    {
      glGenTextures(1, &id_);
    }

    ~Texture()
    {
      glDeleteTextures(1, &id_);
    }

    void use()
    {
      glBindTexture(target_, id_);
    }

    void load_image_texture(std::span<const uint8_t> data)
    {
      if (target_ > 0)
      {
        SPDLOG_ERROR("Texture is already loaded.");
        return;
      }
      target_ = GL_TEXTURE_2D;

      StbiImage texture(data);
      if (!texture.stbi_ptr)
      {
        SPDLOG_ERROR("Failed to load texture image.");
        return;
      }

      glBindTexture(target_, id_);
      glTexParameteri(target_, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(target_, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(target_, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(target_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexImage2D(target_, 0,
        texture.format, texture.width, texture.height,
        0, texture.format, GL_UNSIGNED_BYTE, texture.stbi_ptr);
      glGenerateMipmap(target_);
    }

    void load_cubemap_texture(const std::array<std::span<const uint8_t>, kCubemapSize>& data)
    {
      if (target_ > 0)
      {
        SPDLOG_ERROR("Texture is already loaded.");
        return;
      }
      target_ = GL_TEXTURE_CUBE_MAP;

      StbiImage texture[kCubemapSize] = {
        data[0], data[1], data[2],
        data[3], data[4], data[5],
      };
      for (int i = 0; i < kCubemapSize; i++)
      {
        if (!texture[i].stbi_ptr)
        {
          SPDLOG_ERROR("Failed to load cubemap texture. ({})", i);
          return;
        }
      }

      glBindTexture(target_, id_);

      glTexParameteri(target_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(target_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(target_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(target_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(target_, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

      for (int i = 0; i < kCubemapSize; i++)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
          texture[i].format, texture[i].width, texture[i].height,
          0, texture[i].format, GL_UNSIGNED_BYTE, texture[i].stbi_ptr);
    }
  };

}
