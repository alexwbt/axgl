#pragma once

#include <axgl/common.hpp>

#include <opengl/framebuffer.hpp>
#include <opengl/texture.hpp>

namespace axgl::impl::opengl::renderer
{

struct Screen
{
  std::unique_ptr<::opengl::Texture> screen_texture;
  std::unique_ptr<::opengl::Texture> depth_texture;
  std::unique_ptr<::opengl::Framebuffer> screen_framebuffer;

  void setup(const glm::ivec2& viewport)
  {
    screen_texture = std::make_unique<::opengl::Texture>();
    screen_texture->load_texture(
      0, GL_RGBA16F, viewport.x, viewport.y, 0, GL_RGBA, GL_HALF_FLOAT, nullptr
    );
    screen_texture->set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    screen_texture->set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    depth_texture = std::make_unique<::opengl::Texture>();
    depth_texture->load_texture(
      0, GL_DEPTH_COMPONENT24, viewport.x, viewport.y, 0, GL_DEPTH_COMPONENT,
      GL_FLOAT, nullptr
    );
    screen_framebuffer = std::make_unique<::opengl::Framebuffer>();
    screen_framebuffer->attach_texture(GL_COLOR_ATTACHMENT0, *screen_texture);
    screen_framebuffer->attach_texture(GL_DEPTH_ATTACHMENT, *depth_texture);
    screen_framebuffer->check_status_complete(
      "axgl::impl::opengl::renderer::Screen -> screen_framebuffer"
    );
  }
};

} // namespace axgl::impl::opengl::renderer
