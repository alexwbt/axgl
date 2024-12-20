#pragma once

#include <axgl/common.hpp>
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

  void load_texture(std::span<const uint8_t> data) override
  {
    texture_.load_image_texture(data);
  }
};

NAMESPACE_AXGL_IMPL_END
