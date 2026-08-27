#pragma once

#include <axgl/common.hpp>

#include <opengl/framebuffer.hpp>
#include <opengl/texture.hpp>

namespace axgl::impl::opengl::renderer
{

struct Blend
{
  static constexpr glm::vec4 zero_filler{0.0f};
  static constexpr glm::vec4 one_filler{1.0f};

  bool enabled = true;
  std::unique_ptr<::opengl::Texture> accum_texture;
  std::unique_ptr<::opengl::Texture> reveal_texture;
  std::unique_ptr<::opengl::Framebuffer> blend_framebuffer;

  void setup(const glm::ivec2& viewport, ::opengl::Texture& depth_texture)
  {
    accum_texture = std::make_unique<::opengl::Texture>();
    accum_texture->load_texture(
      0, GL_RGBA16F, viewport.x, viewport.y, 0, GL_RGBA, GL_HALF_FLOAT, nullptr
    );
    accum_texture->set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    accum_texture->set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    reveal_texture = std::make_unique<::opengl::Texture>();
    reveal_texture->load_texture(
      0, GL_R8, viewport.x, viewport.y, 0, GL_RED, GL_FLOAT, nullptr
    );
    reveal_texture->set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    reveal_texture->set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    blend_framebuffer = std::make_unique<::opengl::Framebuffer>();
    blend_framebuffer->attach_texture(GL_COLOR_ATTACHMENT0, *accum_texture);
    blend_framebuffer->attach_texture(GL_COLOR_ATTACHMENT1, *reveal_texture);
    blend_framebuffer->attach_texture(GL_DEPTH_ATTACHMENT, depth_texture);
    blend_framebuffer->set_draw_buffers(
      {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1}
    );
    blend_framebuffer->check_status_complete();
  }

  void reset()
  {
    blend_framebuffer.reset();
    reveal_texture.reset();
    accum_texture.reset();
  }

  void update(const glm::ivec2& viewport, ::opengl::Texture& depth_texture)
  {
    if (enabled && !blend_framebuffer) setup(viewport, depth_texture);
    else if (!enabled && blend_framebuffer) reset();
  }
};

} // namespace axgl::impl::opengl::renderer
