#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/renderer.hpp>

#include <opengl/texture.hpp>

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
    // texture_.set_parameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
    // texture_.set_parameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
    // texture_.set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // texture_.set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // texture_.generate_mipmap();
  }
};

NAMESPACE_AXGL_IMPL_END
