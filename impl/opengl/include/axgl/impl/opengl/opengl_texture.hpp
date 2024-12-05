#pragma once

#include <axgl/axgl.hpp>
#include <axgl/namespace.hpp>
#include <axgl/interface/renderer.hpp>

#include "opengl/texture.hpp"

NAMESPACE_AXGL_IMPL

class OpenglTexture : public interface::Texture
{
private:
  opengl::Texture texture_;

public:
  void use()
  {
    texture_.use();
  }

  void load_texture(Type type, std::span<const uint8_t> data) override
  {
    Texture::type = type;
    texture_.load_2d_texture(data);
  }
};

NAMESPACE_AXGL_IMPL_END
