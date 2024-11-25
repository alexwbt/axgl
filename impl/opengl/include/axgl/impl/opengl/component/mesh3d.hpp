#pragma once

#include <axgl/axgl.hpp>
#include <axgl/namespace.hpp>
#include <axgl/interface/component/mesh.hpp>

#include "opengl/shader_program.hpp"
#include "axgl_opengl_impl/res.hpp"

NAMESPACE_AXGL_IMPL

class OpenglMesh3D : public interface::Mesh3D
{
private:
  std::shared_ptr<OpenglRenderer> renderer_;

  opengl::ShaderProgram shader_{ {
    { GL_VERTEX_SHADER, axgl_opengl_impl_res::get("shader/mesh3d.vs") },
    { GL_FRAGMENT_SHADER, axgl_opengl_impl_res::get("shader/mesh3d.fs") }
  } };
  opengl::VertexArrayObject vertex_array_;
  glm::vec3 color_{ 1.0f, 1.0f, 1.0f };

public:
  OpenglMesh3D(std::shared_ptr<OpenglRenderer> renderer)
    : renderer_(std::move(renderer))
  {}

  void update() override {}

  void render() const override
  {
    shader_.use_program();
    shader_.set_vec3("mesh_color", color_);

    vertex_array_.draw();
  }

  void set_vertices(const std::vector<glm::vec3>& vertices) override
  {
    std::vector<opengl::VertexAttribute> attributes{
      { 3, GL_FLOAT, GL_TRUE, sizeof(glm::vec3), 0 }
    };
    vertex_array_.create_vertex_buffer<glm::vec3>(vertices, attributes);
  }

  void set_normals(const std::vector<glm::vec3>& normals) override
  {
    std::vector<opengl::VertexAttribute> attributes{
      { 3, GL_FLOAT, GL_TRUE, sizeof(glm::vec3), 0 }
    };
    vertex_array_.create_vertex_buffer<glm::vec3>(normals, attributes);
  }

  void set_indices(const std::vector<uint32_t>& indices) override
  {
    vertex_array_.create_element_buffer(indices);
  }

  void set_color(const glm::vec3& color) override
  {
    color_ = color;
  }
};

NAMESPACE_AXGL_IMPL_END

NAMESPACE_AXGL_INTERFACE

template<>
std::shared_ptr<Mesh3D> Realm::create_component()
{
  auto renderer = dynamic_pointer_cast<impl::OpenglRenderer>(get_renderer());
  if (!renderer)
    throw std::runtime_error("Failed to get renderer! "
      "OpenglRenderer is required before creating OpenglMesh3D.");

  return std::make_shared<impl::OpenglMesh3D>(renderer);
}

NAMESPACE_AXGL_INTERFACE_END
