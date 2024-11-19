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

  size_t vertices_size_;
  size_t indices_size_;

  opengl::ShaderProgram shader_{ {
    { GL_VERTEX_SHADER, axgl_opengl_impl_res::get("shader/mesh2d.vs") },
    { GL_FRAGMENT_SHADER, axgl_opengl_impl_res::get("shader/mesh2d.fs") }
  } };

public:
  OpenglMesh2D()
  {
    glGenVertexArrays(1, &vao_id_);
  }

  void update() override {}

  void render() const override
  {
    shader_.use_program();

    glBindVertexArray(vao_id_);
    glDrawArrays(GL_TRIANGLES, 0, vertices_size_);
  }

  void set_data(const std::vector<interface::Vertex2D>& data) override
  {
    vertices_size_ = data.size();

    glBindVertexArray(vao_id_);

    glGenBuffers(1, &vbo_id_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id_);
    glBufferData(GL_ARRAY_BUFFER, vertices_size_ * sizeof(interface::Vertex2D), data.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_TRUE, sizeof(interface::Vertex2D), 0);

    glBindVertexArray(0);
  }

  void set_indices(const std::vector<uint32_t>& indices) override
  {

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
