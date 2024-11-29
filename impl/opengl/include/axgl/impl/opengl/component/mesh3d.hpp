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
  opengl::ShaderProgram shader_{ {
    { GL_VERTEX_SHADER, axgl_opengl_impl_res::get("shader/mesh3d.vs") },
    { GL_FRAGMENT_SHADER, axgl_opengl_impl_res::get("shader/mesh3d.fs") }
  } };
  opengl::VertexArrayObject vertex_array_;
  glm::vec3 color_{ 1.0f, 1.0f, 1.0f };
  float specular_ = 1.0f;

public:
  void render() override
  {
    auto context = get_context();
    glm::mat4 m = model();
    glm::mat4 mvp = context->pv * m;

    shader_.use_program();
    shader_.set_mat4("mvp", mvp);
    shader_.set_mat4("model", m);
    shader_.set_vec3("mesh_color", color_);
    shader_.set_float("mesh_specular", specular_);
    shader_.set_vec3("camera_pos", context->realm->camera.position);
    shader_.set_int("sun_lights_size", 0);
    shader_.set_int("spot_lights_size", 0);
    shader_.set_int("point_lights_size", 0);

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

NAMESPACE_AXGL

template<>
std::shared_ptr<interface::Mesh3D> Axgl::create_component()
{
  return std::make_shared<impl::OpenglMesh3D>();
}

NAMESPACE_AXGL_END
