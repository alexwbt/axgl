#pragma once

#include <axgl/axgl.hpp>
#include <axgl/namespace.hpp>
#include <axgl/interface/component/mesh.hpp>

#include "opengl/shader_program.hpp"
#include "opengl/vertex_array_object.hpp"

#include "axgl_opengl_impl/res.hpp"

NAMESPACE_AXGL_IMPL

class OpenglMesh2D : public interface::Mesh2D
{
private:
  opengl::ShaderProgram shader_{ {
    { GL_VERTEX_SHADER, axgl_opengl_impl_res::get("shader/mesh2d.vs") },
    { GL_FRAGMENT_SHADER, axgl_opengl_impl_res::get("shader/mesh2d.fs") }
  } };
  opengl::VertexArrayObject vertex_array_;
  glm::vec3 color_{ 1.0f, 1.0f, 1.0f };
  glm::vec2 offset_{ 0.0f };
  float scale_ = 1.0f;

public:
  void update(const interface::RealmContext& context) override {}

  void render(const interface::RealmContext& context) override
  {
    auto renderer = dynamic_cast<OpenglRenderer*>(context.renderer);

    shader_.use_program();
    shader_.set_float("scale", scale_);
    shader_.set_vec2("offset", offset_);
    shader_.set_vec2("viewport", renderer->viewport());
    shader_.set_vec3("mesh_color", color_);

    vertex_array_.draw();
  }

  void set_vertices(const std::vector<glm::vec2>& vertices) override
  {
    std::vector<opengl::VertexAttribute> attributes{
      { 2, GL_FLOAT, GL_TRUE, sizeof(glm::vec2), 0 }
    };
    vertex_array_.create_vertex_buffer<glm::vec2>(vertices, attributes);
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

NAMESPACE_AXGL

template<>
std::shared_ptr<interface::Mesh2D> Axgl::create_component()
{
  return std::make_shared<impl::OpenglMesh2D>();
}

NAMESPACE_AXGL_END
