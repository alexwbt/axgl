#pragma once

#include <glad/glad.h>

namespace opengl
{

class Renderbuffer final
{
  GLuint id_ = 0;
  GLuint target_ = 0;
  GLsizei width_ = 0;
  GLsizei height_ = 0;

public:
  Renderbuffer() { glGenRenderbuffers(1, &id_); }

  Renderbuffer(const Renderbuffer&) = delete;
  Renderbuffer& operator=(const Renderbuffer&) = delete;

  Renderbuffer(Renderbuffer&& other) noexcept
  {
    id_ = other.id_;
    target_ = other.target_;
    width_ = other.width_;
    height_ = other.height_;
    other.id_ = 0;
    other.target_ = 0;
    other.width_ = 0;
    other.height_ = 0;
  }
  Renderbuffer& operator=(Renderbuffer&& other) noexcept
  {
    if (this != &other)
    {
      if (id_ > 0) glDeleteRenderbuffers(1, &id_);

      id_ = other.id_;
      target_ = other.target_;
      width_ = other.width_;
      height_ = other.height_;
      other.id_ = 0;
      other.target_ = 0;
      other.width_ = 0;
      other.height_ = 0;
    }
    return *this;
  }

  ~Renderbuffer()
  {
    if (id_ > 0) glDeleteRenderbuffers(1, &id_);
  }

  [[nodiscard]] GLuint get_id() const { return id_; }

  [[nodiscard]] GLuint get_target() const { return target_; }

  [[nodiscard]] GLsizei get_width() const { return width_; }

  [[nodiscard]] GLsizei get_height() const { return height_; }

  [[nodiscard]] bool loaded() const { return target_ > 0; }

  void use() const { glBindRenderbuffer(target_, id_); }

  void init_multisample_renderbuffer(
    const GLsizei samples, const GLenum internal_format, const GLsizei width, const GLsizei height)
  {
    if (loaded())
    {
      AXGL_LOG_ERROR("Renderbuffer is already initialized.");
      return;
    }
    target_ = GL_RENDERBUFFER;
    width_ = width;
    height_ = height;

    use();
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, internal_format, width, height);
  }
};

} // namespace opengl
