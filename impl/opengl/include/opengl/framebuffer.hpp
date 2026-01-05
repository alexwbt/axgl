#pragma once

#include <axgl/common.hpp>
#include <glad/glad.h>
#include <opengl/renderbuffer.hpp>
#include <opengl/texture.hpp>

namespace opengl
{

class Framebuffer final
{
  GLuint id_ = 0;

public:
  Framebuffer() { glGenFramebuffers(1, &id_); }

  Framebuffer(const Framebuffer&) = delete;
  Framebuffer& operator=(const Framebuffer&) = delete;

  Framebuffer(Framebuffer&& other) noexcept
  {
    id_ = other.id_;
    other.id_ = 0;
  }
  Framebuffer& operator=(Framebuffer&& other) noexcept
  {
    if (this != &other)
    {
      if (id_ > 0) glDeleteFramebuffers(1, &id_);

      id_ = other.id_;
      other.id_ = 0;
    }
    return *this;
  }

  ~Framebuffer()
  {
    if (id_ > 0) glDeleteFramebuffers(1, &id_);
  }

  void use() const { glBindFramebuffer(GL_FRAMEBUFFER, id_); }

  void attach_texture(const GLenum attachment, const ::opengl::Texture& texture) const
  {
    use();
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, texture.get_target(), texture.get_id(), 0);
  }

  void attach_renderbuffer(const GLenum attachment, const ::opengl::Renderbuffer& renderbuffer) const
  {
    use();
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, renderbuffer.get_target(), renderbuffer.get_id());
  }

  void set_draw_buffers(const std::vector<GLenum>& attachments) const
  {
    use();
    glDrawBuffers(static_cast<GLsizei>(attachments.size()), attachments.data());

    if (const auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER); status != GL_FRAMEBUFFER_COMPLETE)
      AXGL_LOG_ERROR("Framebuffer status is incomplete. ({})", status);
  }
};

} // namespace opengl
