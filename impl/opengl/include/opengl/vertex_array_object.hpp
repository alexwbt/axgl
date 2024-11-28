#pragma once

#include <vector>
#include <stdexcept>

#include "opengl/buffer_object.hpp"

namespace opengl
{

  class VertexArrayObject final
  {
  private:
    GLuint id_ = 0;
    std::vector<const BufferObject*> buffer_objects_;

    size_t vertex_size_ = 0;
    size_t element_size_ = 0;
    size_t attribute_size_ = 0;

  public:
    VertexArrayObject()
    {
      glGenVertexArrays(1, &id_);
    }

    ~VertexArrayObject()
    {
      for (auto buffer_object : buffer_objects_)
        delete buffer_object;

      glDeleteVertexArrays(1, &id_);
    }

    template <typename VertexType>
    void create_vertex_buffer(
      const std::span<const VertexType>& data,
      const std::span<const VertexAttribute>& attributes)
    {
      use();
      auto buffer = new VertexBufferObject(data, attributes, attribute_size_);

      if (vertex_size_ > 0 && buffer->size() != vertex_size_)
        throw std::runtime_error("Size of all vertex buffer should be equal.");

      buffer_objects_.push_back(buffer);
      vertex_size_ = buffer->size();
      attribute_size_ += buffer->attribute_size();
    }

    void create_element_buffer(const std::span<const uint32_t>& data)
    {
      use();
      auto buffer = new ElementBufferObject(data);

      buffer_objects_.push_back(buffer);
      element_size_ = buffer->size();
    }

    void draw() const
    {
      use();
      if (element_size_ > 0)
        glDrawElements(GL_TRIANGLES, element_size_, GL_UNSIGNED_INT, 0);
      else if (vertex_size_ > 0)
        glDrawArrays(GL_TRIANGLES, 0, vertex_size_);
    }

  private:
    void use() const
    {
      glBindVertexArray(id_);
    }
  };

}
