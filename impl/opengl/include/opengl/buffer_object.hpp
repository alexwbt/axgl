#pragma once

#include <cstdint>
#include <span>

#include <glad/glad.h>

namespace opengl
{

class BufferObject
{
private:
  GLuint id_;
  size_t size_;

public:
  template <typename DataType>
  BufferObject(GLenum target, const std::span<const DataType>& data, GLenum usage)
  {
    size_ = data.size();

    glGenBuffers(1, &id_);
    glBindBuffer(target, id_);
    glBufferData(target, size_ * sizeof(DataType), data.data(), usage);
  }

  BufferObject(const BufferObject&) = delete;
  BufferObject& operator=(const BufferObject&) = delete;

  BufferObject(BufferObject&& other) noexcept
  {
    id_ = other.id_;
    size_ = other.size_;
    other.id_ = 0;
    other.size_ = 0;
  }
  BufferObject& operator=(BufferObject&& other) noexcept
  {
    if (this != &other)
    {
      if (id_ > 0)
        glDeleteBuffers(1, &id_);

      id_ = other.id_;
      size_ = other.size_;
      other.id_ = 0;
      other.size_ = 0;
    }
    return *this;
  }

  virtual ~BufferObject()
  {
    if (id_ > 0)
      glDeleteBuffers(1, &id_);
  }

  [[nodiscard]] size_t size() const { return size_; }
};

struct VertexAttribute
{
  GLint size;
  GLenum type;
  GLboolean normalized;
  GLsizei stride;
  const void* pointer;
};

class VertexBufferObject final : public BufferObject
{
private:
  size_t attribute_size_ = 0;

public:
  template <typename VertexType>
  VertexBufferObject(const std::span<const VertexType>& data, GLenum usage) : BufferObject(GL_ARRAY_BUFFER, data, usage)
  {
  }

  VertexBufferObject(const VertexBufferObject&) = delete;
  VertexBufferObject& operator=(const VertexBufferObject&) = delete;

  void set_attributes(const std::span<const VertexAttribute>& attributes, int attribute_offset = 0)
  {
    if (attribute_size_ > 0)
    {
      SPDLOG_ERROR("Attributes are already set.");
      return;
    }

    attribute_size_ = attributes.size();
    for (int i = 0; i < attribute_size_; i++)
    {
      auto index = attribute_offset + i;
      const auto& attr = attributes[i];
      glEnableVertexAttribArray(index);
      switch (attr.type)
      {
      case GL_BYTE:
      case GL_UNSIGNED_BYTE:
      case GL_SHORT:
      case GL_UNSIGNED_SHORT:
      case GL_INT:
      case GL_UNSIGNED_INT: glVertexAttribIPointer(index, attr.size, attr.type, attr.stride, attr.pointer); break;
      case GL_HALF_FLOAT:
      case GL_FLOAT:
      case GL_FIXED:
      case GL_INT_2_10_10_10_REV:
      case GL_UNSIGNED_INT_2_10_10_10_REV:
      case GL_UNSIGNED_INT_10F_11F_11F_REV:
        glVertexAttribPointer(index, attr.size, attr.type, attr.normalized, attr.stride, attr.pointer);
        break;
        // case GL_DOUBLE:
        //   glVertexAttribLPointer(index, attr.size, attr.type, attr.normalized, attr.stride, attr.pointer);
        //   break;
      }
    }
  }

  [[nodiscard]] size_t attribute_size() const { return attribute_size_; }
};

class ElementBufferObject final : public BufferObject
{
public:
  ElementBufferObject(const std::span<const uint32_t>& data, GLenum usage) :
    BufferObject(GL_ELEMENT_ARRAY_BUFFER, data, usage)
  {
  }

  ElementBufferObject(const ElementBufferObject&) = delete;
  ElementBufferObject& operator=(const ElementBufferObject&) = delete;
};

} // namespace opengl
