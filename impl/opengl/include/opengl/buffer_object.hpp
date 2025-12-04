#pragma once

#include <span>

#include <glad/glad.h>

namespace opengl
{

class BufferObject final
{
  GLuint id_ = 0;
  GLenum target_ = 0;
  std::size_t size_ = 0;

public:
  explicit BufferObject(const GLenum target) : target_(target) { glGenBuffers(1, &id_); }
  BufferObject(const BufferObject&) = delete;
  BufferObject& operator=(const BufferObject&) = delete;
  BufferObject(BufferObject&& other) noexcept
  {
    id_ = other.id_;
    size_ = other.size_;
    target_ = other.target_;
    other.id_ = 0;
    other.size_ = 0;
    other.target_ = 0;
  }
  BufferObject& operator=(BufferObject&& other) noexcept
  {
    if (this != &other)
    {
      if (id_ > 0)
        glDeleteBuffers(1, &id_);

      id_ = other.id_;
      size_ = other.size_;
      target_ = other.target_;
      other.id_ = 0;
      other.size_ = 0;
      other.target_ = 0;
    }
    return *this;
  }
  ~BufferObject()
  {
    if (id_ > 0)
      glDeleteBuffers(1, &id_);
  }

  template <typename DataType>
  void set_data(const std::span<const DataType>& data, const GLenum usage)
  {
    bind();
    size_ = data.size();
    glBufferData(target_, size_ * sizeof(DataType), data.data(), usage);
  }

  void bind() const { glBindBuffer(target_, id_); }

  [[nodiscard]] size_t size() const { return size_; }
};

} // namespace opengl
