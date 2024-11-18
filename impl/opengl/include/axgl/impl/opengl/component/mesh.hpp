#pragma once

#include <axgl/axgl.hpp>
#include <axgl/namespace.hpp>
#include <axgl/interface/component/mesh.hpp>

#include "opengl/shader_program.hpp"

namespace opengl
{
  constexpr std::string_view MESH2D_VERTEX_SHADER_SOURCE =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
  constexpr std::string_view MESH2D_FRAGMENT_SHADER_SOURCE =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";
}

NAMESPACE_AXGL_IMPL

class OpenglMesh2D : public interface::Mesh2D
{
private:
  GLuint vao_id_ = 0;
  GLuint vbo_id_ = 0;
  GLuint ebo_id_ = 0;

  opengl::ShaderProgram shader_({
    {GL_VERTEX_SHADER, MESH2D_VERTEX_SHADER_SOURCE},
    {GL_FRAGMENT_SHADER, MESH2D_FRAGMENT_SHADER_SOURCE} })

public:
  OpenglMesh2D()
  {
    glGenVertexArrays(1, &vao_id_);
  }

  void update() override {}

  void render() const override {}

  void set_data(const std::vector<interface::Vertex2D>& data) override
  {
    glBindVertexArray(vao_id_);

    glGenBuffers(1, &vbo_id_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id_);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(interface::Vertex2D), data.data(), GL_STATIC_DRAW);

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
