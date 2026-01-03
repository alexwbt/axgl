#pragma once

#include <cstdint>
#include <memory>
#include <span>

#include <axgl/interface/texture.hpp>

#include <opengl/texture.hpp>

namespace axgl::impl::opengl
{

class Texture : public axgl::Texture
{
  axgl::ptr_t<::opengl::Texture> texture_;

public:
  Texture() : texture_(axgl::create_ptr<::opengl::Texture>()) { }

  void use(const GLenum texture_unit) const { texture_->use(texture_unit); }

  void load_texture(const std::span<const std::uint8_t> data) override
  {
    texture_->load_image_texture(data);
    texture_->set_parameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
    texture_->set_parameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
    texture_->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    texture_->set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    texture_->generate_mipmap();
  }

  [[nodiscard]] std::uint32_t get_width() override { return texture_->get_width(); }
  [[nodiscard]] std::uint32_t get_height() override { return texture_->get_height(); }

  void replace_texture(axgl::ptr_t<::opengl::Texture> texture) { texture_ = std::move(texture); }
  [[nodiscard]] axgl::ptr_t<::opengl::Texture> get_texture() const { return texture_; }
};

} // namespace axgl::impl::opengl

