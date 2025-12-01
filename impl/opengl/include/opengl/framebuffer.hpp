#pragma once

#include <glad/glad.h>
#include <spdlog/spdlog.h>

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
      if (id_ > 0)
        glDeleteFramebuffers(1, &id_);

      id_ = other.id_;
      other.id_ = 0;
    }
    return *this;
  }

  ~Framebuffer()
  {
    if (id_ > 0)
      glDeleteFramebuffers(1, &id_);
  }

  void use() const { glBindFramebuffer(GL_FRAMEBUFFER, id_); }

  void attach_texture(const GLuint attachment, const Texture& texture) const
  {
    use();
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment, texture.get_id(), 0);
  }

  void set_draw_buffers(const std::vector<GLuint>& attachments) const
  {
    std::vector<GLenum> buffers(attachments.size());
    for (int i = 0; i < attachments.size(); i++)
      buffers[i] = GL_COLOR_ATTACHMENT0 + attachments[i];

    use();
    glDrawBuffers(buffers.size(), buffers.data());

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      SPDLOG_ERROR("Framebuffer status is incomplete.");
  }
};

} // namespace opengl
