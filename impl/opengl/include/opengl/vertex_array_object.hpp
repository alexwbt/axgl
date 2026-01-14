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
  GLenum mode_ = GL_TRIANGLES;
  std::vector<std::unique_ptr<BufferObject>> buffer_objects_;

  size_t vertex_size_ = 0;
  size_t element_size_ = 0;
  size_t attribute_size_ = 0;

public:
  explicit VertexArrayObject() { glGenVertexArrays(1, &id_); }
  VertexArrayObject(const VertexArrayObject&) = delete;
  VertexArrayObject& operator=(const VertexArrayObject&) = delete;

  VertexArrayObject(VertexArrayObject&& other) noexcept
  {
    id_ = other.id_;
    other.id_ = 0;
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
      if (id_ > 0) glDeleteVertexArrays(1, &id_);

      id_ = other.id_;
      other.id_ = 0;
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
    if (id_ > 0) glDeleteVertexArrays(1, &id_);
  }

  template <typename VertexType>
  auto create_vertex_buffer(
    const std::span<const VertexType>& data,
    const std::span<const VertexAttribute>& attributes,
    const int attributes_offset,
    const GLuint divisor = 0)
  {
    bind();
    auto buffer = std::make_unique<BufferObject>(GL_ARRAY_BUFFER);
    buffer->set_data(data, GL_STATIC_DRAW);
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
      if (divisor > 0) glVertexAttribDivisor(index, divisor);
    }

    if (divisor == 0)
    {
      if (vertex_size_ > 0 && buffer->size() != vertex_size_)
        throw std::runtime_error("Size of all vertex buffer should be equal.");
      vertex_size_ = buffer->size();
    }

    const auto id = buffer->id();

    attribute_size_ += attributes.size();
    buffer_objects_.emplace_back(std::move(buffer));

    return id;
  }

  auto create_element_buffer(const std::span<const uint32_t>& data)
  {
    bind();
    auto buffer = std::make_unique<BufferObject>(GL_ELEMENT_ARRAY_BUFFER);
    buffer->set_data(data, GL_STATIC_DRAW);

    const auto id = buffer->id();

    element_size_ = buffer->size();
    buffer_objects_.emplace_back(std::move(buffer));

    return id;
  }

  template <typename DataType>
  void update_buffer_data(const GLuint buffer_id, const std::span<const DataType>& data)
  {
    for (const auto& buffer : buffer_objects_)
    {
      if (buffer->id() == buffer_id)
      {
        buffer->set_data(data, GL_DYNAMIC_DRAW);
        break;
      }
    }
  }

  void set_mode(const GLenum mode) { mode_ = mode; }

  void draw() const
  {
    bind();
    if (element_size_ > 0)
      glDrawElements(mode_, static_cast<GLsizei>(element_size_), GL_UNSIGNED_INT, nullptr);
    else if (vertex_size_ > 0)
      glDrawArrays(mode_, 0, static_cast<GLint>(vertex_size_));
  }

  void draw_instanced(const GLsizei count) const
  {
    bind();
    if (element_size_ > 0)
      glDrawElementsInstanced(mode_, static_cast<GLsizei>(element_size_), GL_UNSIGNED_INT, nullptr, count);
    else if (vertex_size_ > 0)
      glDrawArraysInstanced(mode_, 0, static_cast<GLint>(vertex_size_), count);
  }

private:
  void bind() const { glBindVertexArray(id_); }
};

} // namespace opengl
