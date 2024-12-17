#pragma once

#include <axgl/namespace.hpp>
#include <axgl/impl/assimp/texture.hpp>

#include "opengl/texture.hpp"

NAMESPACE_AXGL_IMPL

class OpenglTexture : public AssimpTexture
{
private:
  opengl::Texture texture_;

public:
  void use()
  {
    texture_.use();
  }

  void load_texture(std::span<const uint8_t> data) override
  {
    texture_.load_image_texture(data);
  }

  void load_texture(aiTexture* data) override
  {
    texture_.load_assimp_texture(data);
  }
};

NAMESPACE_AXGL_IMPL_END
