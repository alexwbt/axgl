#pragma once

#include <vector>
#include <memory>
#include <stdexcept>

#include <opengl/buffer_object.hpp>

namespace opengl
{

class VertexArrayObject final
{
private:
  GLuint id_;
  GLenum usage_;
  std::vector<std::unique_ptr<const BufferObject>> buffer_objects_;

  size_t vertex_size_ = 0;
  size_t element_size_ = 0;
  size_t attribute_size_ = 0;

public:
  VertexArrayObject(GLenum usage = GL_STATIC_DRAW) : usage_(usage) { glGenVertexArrays(1, &id_); }
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
  int create_vertex_buffer(
    const std::span<const VertexType>& data, const std::span<const VertexAttribute>& attributes, int attributes_offset)
  {
    use();
    auto buffer = std::make_unique<VertexBufferObject>(data, usage_);
    buffer->set_attributes(attributes, attributes_offset);

    if (vertex_size_ > 0 && buffer->size() != vertex_size_)
      throw std::runtime_error("Size of all vertex buffer should be equal.");

    vertex_size_ = buffer->size();
    attribute_size_ += buffer->attribute_size();
    buffer_objects_.push_back(std::move(buffer));

    return buffer_objects_.size() - 1;
  }

  int create_element_buffer(const std::span<const uint32_t>& data)
  {
    use();
    auto buffer = std::make_unique<ElementBufferObject>(data, usage_);

    element_size_ = buffer->size();
    buffer_objects_.push_back(std::move(buffer));

    return buffer_objects_.size() - 1;
  }

  // const BufferObject& get_buffer_object()
  // {

  // }

  void draw() const
  {
    use();
    if (element_size_ > 0)
      glDrawElements(GL_TRIANGLES, element_size_, GL_UNSIGNED_INT, 0);
    else if (vertex_size_ > 0)
      glDrawArrays(GL_TRIANGLES, 0, vertex_size_);
  }

private:
  void use() const { glBindVertexArray(id_); }
};

} // namespace opengl
