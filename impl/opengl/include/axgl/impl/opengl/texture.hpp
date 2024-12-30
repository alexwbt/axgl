#pragma once

#include <memory>

#include <axgl/common.hpp>
#include <axgl/interface/service.hpp>
#include <axgl/interface/renderer.hpp>

#include <opengl/texture.hpp>

NAMESPACE_AXGL_IMPL

class OpenglTexture : public interface::Texture
{
private:
  std::shared_ptr<opengl::Texture> texture_;

public:
  OpenglTexture()
  {
    texture_ = std::make_shared<opengl::Texture>();
  }

  void use()
  {
    texture_->use();
  }

  void load_texture(std::span<const uint8_t> data) override
  {
    texture_->load_image_texture(data);
    texture_->set_parameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
    texture_->set_parameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
    texture_->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    texture_->set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    texture_->generate_mipmap();
  }

  void replace_texture(std::shared_ptr<opengl::Texture> texture)
  {
    texture_ = std::move(texture);
  }
};

NAMESPACE_AXGL_IMPL_END
