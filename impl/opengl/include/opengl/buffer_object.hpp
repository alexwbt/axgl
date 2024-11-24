#pragma once

#include <span>
#include <stdint.h>

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
    BufferObject(GLenum target, const std::span<const DataType>& data)
    {
      size_ = data.size();

      glGenBuffers(1, &id_);
      glBindBuffer(target, id_);
      glBufferData(target, size_ * sizeof(DataType), data.data(), GL_STATIC_DRAW);
    }

    virtual ~BufferObject()
    {
      glDeleteBuffers(1, &id_);
    }

    size_t size() const { return size_; }
  };

  struct VertexAttribute
  {
    GLint size;
    GLenum type;
    GLboolean normalized;
    GLsizei stride;
    const void* pointer;
  };

  class VertexBufferObject : public BufferObject
  {
  private:
    size_t attribute_size_;

  public:
    template <typename VertexType>
    VertexBufferObject(
      const std::span<const VertexType>& data,
      const std::span<const VertexAttribute>& attributes,
      int offset = 0
    ) :
      BufferObject(GL_ARRAY_BUFFER, data),
      attribute_size_(attributes.size())
    {
      for (int i = 0; i < attribute_size_; ++i)
      {
        const auto& attr = attributes[i];
        glEnableVertexAttribArray(offset + i);
        switch (attr.type)
        {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
        case GL_INT:
        case GL_UNSIGNED_INT:
          glVertexAttribIPointer(i, attr.size, attr.type, attr.stride, attr.pointer);
          break;
        case GL_HALF_FLOAT:
        case GL_FLOAT:
        case GL_FIXED:
        case GL_INT_2_10_10_10_REV:
        case GL_UNSIGNED_INT_2_10_10_10_REV:
        case GL_UNSIGNED_INT_10F_11F_11F_REV:
          glVertexAttribPointer(i, attr.size, attr.type, attr.normalized, attr.stride, attr.pointer);
          break;
          // case GL_DOUBLE:
          //   glVertexAttribLPointer(i, attr.size, attr.type, attr.normalized, attr.stride, attr.pointer);
          //   break;
        }
      }
    }

    size_t attribute_size() const { return attribute_size_; }
  };

  class ElementBufferObject : public BufferObject
  {
  public:
    ElementBufferObject(const std::span<const uint32_t>& data)
      : BufferObject(GL_ELEMENT_ARRAY_BUFFER, data)
    {}
  };

}
