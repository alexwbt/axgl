#pragma once

#include <axgl/common.hpp>

#include <opengl/framebuffer.hpp>
#include <opengl/texture.hpp>

namespace axgl::impl::opengl::renderer
{

struct Hdr
{
  bool enabled = false;
  float exposure = 1.0f;
  std::unique_ptr<::opengl::Texture> hdr_texture;
  std::unique_ptr<::opengl::Framebuffer> hdr_framebuffer;

  void setup(const glm::ivec2& viewport)
  {
    hdr_texture = std::make_unique<::opengl::Texture>();
    hdr_texture->load_texture(
      0, GL_RGBA16F, viewport.x, viewport.y, 0, GL_RGBA, GL_HALF_FLOAT, nullptr
    );
    hdr_texture->set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    hdr_texture->set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    hdr_framebuffer = std::make_unique<::opengl::Framebuffer>();
    hdr_framebuffer->attach_texture(GL_COLOR_ATTACHMENT0, *hdr_texture);
    hdr_framebuffer->check_status_complete(
      "axgl::impl::opengl::renderer::Hdr -> hdr_framebuffer"
    );
  }

  void reset()
  {
    hdr_framebuffer.reset();
    hdr_texture.reset();
  }

  void update(const glm::ivec2& viewport)
  {
    if (enabled && !hdr_framebuffer) setup(viewport);
    else if (!enabled && hdr_framebuffer) reset();
  }
};

} // namespace axgl::impl::opengl::renderer
