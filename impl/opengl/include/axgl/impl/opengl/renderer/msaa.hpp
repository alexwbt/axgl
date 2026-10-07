#pragma once

#include <axgl/common.hpp>

#include <opengl/framebuffer.hpp>
#include <opengl/renderbuffer.hpp>
#include <opengl/texture.hpp>

namespace axgl::impl::opengl::renderer {

struct MSAA {
  bool enabled = false;
  GLsizei msaa_sample_count = 4;
  std::unique_ptr<::opengl::Texture> msaa_texture;
  std::unique_ptr<::opengl::Renderbuffer> msaa_depth;
  std::unique_ptr<::opengl::Framebuffer> msaa_framebuffer;

  void setup(const glm::ivec2& viewport) {
    msaa_texture = std::make_unique<::opengl::Texture>();
    msaa_texture->init_multisample_texture(
      msaa_sample_count, GL_RGBA16F, viewport.x, viewport.y, GL_TRUE
    );
    msaa_texture->set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    msaa_texture->set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // The resolve in renderer.hpp passes GL_DEPTH_BUFFER_BIT, and
    // glBlitFramebuffer requires the source and destination depth formats to
    // match exactly (GL_INVALID_OPERATION otherwise, per the spec). The screen
    // framebuffer uses GL_DEPTH_COMPONENT24 (see screen.hpp), so the MSAA depth
    // attachment must use the same format. With GL_DEPTH24_STENCIL8 the whole
    // blit (color included) was rejected on macOS, leaving the scene black;
    // Mesa compares only depth bit counts so it tolerated the mismatch.
    msaa_depth = std::make_unique<::opengl::Renderbuffer>();
    msaa_depth->init_multisample_renderbuffer(
      msaa_sample_count, GL_DEPTH_COMPONENT24, viewport.x, viewport.y
    );
    msaa_framebuffer = std::make_unique<::opengl::Framebuffer>();
    msaa_framebuffer->attach_texture(GL_COLOR_ATTACHMENT0, *msaa_texture);
    msaa_framebuffer->attach_renderbuffer(GL_DEPTH_ATTACHMENT, *msaa_depth);
    msaa_framebuffer->check_status_complete();
  }

  void reset() {
    msaa_framebuffer.reset();
    msaa_depth.reset();
    msaa_texture.reset();
  }

  void update(const glm::ivec2& viewport) {
    if (enabled && !msaa_framebuffer) setup(viewport);
    else if (!enabled && msaa_framebuffer) reset();
  }
};

} // namespace axgl::impl::opengl::renderer
