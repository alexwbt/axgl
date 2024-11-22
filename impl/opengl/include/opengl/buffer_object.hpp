#pragma once

#include <span>
#include <stdint.h>

#include <glad/glad.h>

namespace opengl
{
  struct VertexAttribute
  {
    GLint size;
    GLenum type;
    GLboolean normalized;
    GLsizei stride;
    const void* pointer;
  };

  class BufferObject
  {
  private:
    GLuint id_;
    int size_;

  public:
    template <typename DataType>
    BufferObject(GLenum target, GLenum usage, const std::span<DataType>& data)
    {
      size_ = data.size();

      glGenBuffers(1, &id_);
      glBindBuffer(target, id_);
      glBufferData(target, size_ * sizeof(DataType), data.data(), usage);
    }

    virtual ~BufferObject()
    {
      glDeleteBuffers(1, &id_);
    }
  };

  class VertexBufferObject : public BufferObject
  {
  public:
    template <typename VertexType>
    VertexBufferObject(
      const std::span<VertexType>& data,
      const std::vector<VertexAttribute>& attributes,
      GLenum usage = GL_STATIC_DRAW
    ) : BufferObject(GL_ARRAY_BUFFER, usage, data),
    {
      for (int i = 0; i < attributes.size(); ++i)
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
  };

  class ElementBufferObject : public BufferObject
  {
  public:
    ElementBufferObject(const std::span<uint32_t>& data, GLenum usage = GL_STATIC_DRAW)
      : BufferObject(GL_ELEMENT_ARRAY_BUFFER, usage, data)
    {}
  };

}
