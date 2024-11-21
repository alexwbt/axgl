#pragma once

#include <axgl/axgl.hpp>
#include <axgl/namespace.hpp>
#include <axgl/interface/component/mesh.hpp>

#include "opengl/shader_program.hpp"
#include "axgl_opengl_impl/res.hpp"

NAMESPACE_AXGL_IMPL

class OpenglMesh2D : public interface::Mesh2D
{
private:
  GLuint vao_id_ = 0;
  GLuint vbo_id_ = 0;
  GLuint ebo_id_ = 0;

  size_t vertices_size_ = 0;
  size_t indices_size_ = 0;

  opengl::ShaderProgram shader_{ {
    { GL_VERTEX_SHADER, axgl_opengl_impl_res::get("shader/mesh2d.vs") },
    { GL_FRAGMENT_SHADER, axgl_opengl_impl_res::get("shader/mesh2d.fs") }
  } };

public:
  OpenglMesh2D()
  {
    glGenVertexArrays(1, &vao_id_);
  }

  ~OpenglMesh2D()
  {
    if (vao_id_ > 0) glDeleteVertexArrays(1, &vao_id_);
    if (vbo_id_ > 0) glDeleteBuffers(1, &vbo_id_);
    if (ebo_id_ > 0) glDeleteBuffers(1, &ebo_id_);
  }

  void update() override {}

  void render() const override
  {
    shader_.use_program();

    glBindVertexArray(vao_id_);
    if (indices_size_ > 0)
      glDrawElements(GL_TRIANGLES, indices_size_, GL_UNSIGNED_INT, 0);
    else
      glDrawArrays(GL_TRIANGLES, 0, vertices_size_);
  }

  void set_vertices(const std::vector<interface::Vertex2D>& vertices) override
  {
    vertices_size_ = vertices.size();

    glBindVertexArray(vao_id_);

    glGenBuffers(1, &vbo_id_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id_);
    glBufferData(GL_ARRAY_BUFFER, vertices_size_ * sizeof(interface::Vertex2D), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_TRUE, sizeof(interface::Vertex2D), 0);

    glBindVertexArray(0);
  }

  void set_indices(const std::vector<uint32_t>& indices) override
  {
    indices_size_ = indices.size();

    glBindVertexArray(vao_id_);

    glGenBuffers(1, &ebo_id_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size_ * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
  }
};

NAMESPACE_AXGL_IMPL_END

NAMESPACE_AXGL_INTERFACE

template<>
std::shared_ptr<Mesh2D> Component::create_component()
{
  return std::make_shared<impl::OpenglMesh2D>();
}

NAMESPACE_AXGL_INTERFACE_END
