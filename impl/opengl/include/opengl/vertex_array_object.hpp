#pragma once

#include <memory>
#include <stdexcept>
#include <vector>

#include <opengl/buffer_object.hpp>

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

class VertexArrayObject final
{
  GLuint id_;
  GLenum usage_;
  std::vector<std::unique_ptr<const BufferObject>> buffer_objects_;

  size_t vertex_size_ = 0;
  size_t element_size_ = 0;
  size_t attribute_size_ = 0;

public:
  explicit VertexArrayObject(const GLenum usage = GL_STATIC_DRAW) : usage_(usage) { glGenVertexArrays(1, &id_); }
  VertexArrayObject(const VertexArrayObject&) = delete;
  VertexArrayObject& operator=(const VertexArrayObject&) = delete;

  VertexArrayObject(VertexArrayObject&& other) noexcept
  {
    id_ = other.id_;
    other.id_ = 0;
    usage_ = other.usage_;
    other.usage_ = 0;
    vertex_size_ = other.vertex_size_;
    other.vertex_size_ = 0;
    element_size_ = other.element_size_;
    other.element_size_ = 0;
    attribute_size_ = other.attribute_size_;
    other.attribute_size_ = 0;
    buffer_objects_ = std::move(other.buffer_objects_);
  }
  VertexArrayObject& operator=(VertexArrayObject&& other) noexcept
  {
    if (this != &other)
    {
      buffer_objects_.clear();
      if (id_ > 0)
        glDeleteVertexArrays(1, &id_);

      id_ = other.id_;
      other.id_ = 0;
      usage_ = other.usage_;
      other.usage_ = 0;
      vertex_size_ = other.vertex_size_;
      other.vertex_size_ = 0;
      element_size_ = other.element_size_;
      other.element_size_ = 0;
      attribute_size_ = other.attribute_size_;
      other.attribute_size_ = 0;
      buffer_objects_ = std::move(other.buffer_objects_);
    }
    return *this;
  }

  ~VertexArrayObject()
  {
    buffer_objects_.clear();
    if (id_ > 0)
      glDeleteVertexArrays(1, &id_);
  }

  template <typename VertexType>
  size_t create_vertex_buffer(
    const std::span<const VertexType>& data,
    const std::span<const VertexAttribute>& attributes,
    const int attributes_offset,
    const GLuint divisor = 0)
  {
    bind();
    auto buffer = std::make_unique<BufferObject>(GL_ARRAY_BUFFER);
    buffer->set_data(data, usage_);
    for (int i = 0; i < attributes.size(); i++)
    {
      const auto index = attributes_offset + i;
      const auto& [size, type, normalized, stride, pointer] = attributes[i];
      glEnableVertexAttribArray(index);
      switch (type)
      {
      case GL_BYTE:
      case GL_UNSIGNED_BYTE:
      case GL_SHORT:
      case GL_UNSIGNED_SHORT:
      case GL_INT:
      case GL_UNSIGNED_INT: glVertexAttribIPointer(index, size, type, stride, pointer); break;
      case GL_HALF_FLOAT:
      case GL_FLOAT:
      case GL_FIXED:
      case GL_INT_2_10_10_10_REV:
      case GL_UNSIGNED_INT_2_10_10_10_REV:
      case GL_UNSIGNED_INT_10F_11F_11F_REV:
        glVertexAttribPointer(index, size, type, normalized, stride, pointer);
        break;
        // case GL_DOUBLE:
        //   glVertexAttribLPointer(index, attr.size, attr.type, attr.normalized, attr.stride, attr.pointer);
        //   break;
      default:;
      }
      if (divisor > 0)
        glVertexAttribDivisor(index, divisor);
    }

    if (divisor == 0)
    {
      if (vertex_size_ > 0 && buffer->size() != vertex_size_)
        throw std::runtime_error("Size of all vertex buffer should be equal.");
      vertex_size_ = buffer->size();
    }
    attribute_size_ += attributes.size();
    buffer_objects_.push_back(std::move(buffer));

    return buffer_objects_.size() - 1;
  }

  size_t create_element_buffer(const std::span<const uint32_t>& data)
  {
    bind();
    auto buffer = std::make_unique<BufferObject>(GL_ELEMENT_ARRAY_BUFFER);
    buffer->set_data(data, usage_);

    element_size_ = buffer->size();
    buffer_objects_.push_back(std::move(buffer));

    return buffer_objects_.size() - 1;
  }

  void draw() const
  {
    bind();
    if (element_size_ > 0)
      glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(element_size_), GL_UNSIGNED_INT, nullptr);
    else if (vertex_size_ > 0)
      glDrawArrays(GL_TRIANGLES, 0, static_cast<GLint>(vertex_size_));
  }

  void draw_instanced(const GLsizei count) const
  {
    bind();
    if (element_size_ > 0)
      glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(element_size_), GL_UNSIGNED_INT, nullptr, count);
    else if (vertex_size_ > 0)
      glDrawArraysInstanced(GL_TRIANGLES, 0, static_cast<GLint>(vertex_size_), count);
  }

private:
  void bind() const { glBindVertexArray(id_); }
};

} // namespace opengl
