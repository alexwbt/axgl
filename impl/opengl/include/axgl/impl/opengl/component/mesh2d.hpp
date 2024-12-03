#pragma once

#include <vector>

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
  std::shared_ptr<Texture> texture_;

public:
  void render() override
  {
    auto context = get_context();
    glm::mat4 m = model();
    glm::mat4 mvp = context->pv * m;

    shader_.use_program();
    shader_.set_mat4("mvp", mvp);
    shader_.set_vec3("mesh_color", color_);

    if (texture_)
    {
      glActiveTexture(GL_TEXTURE0);
      texture_->use();
      shader_.set_int("mesh_texture", 0);
      shader_.set_bool("use_texture", true);
    }

    vertex_array_.draw();
  }

  void set_vertices(const std::span<const glm::vec2>& vertices) override
  {
    std::vector<opengl::VertexAttribute> attributes{
      { 2, GL_FLOAT, GL_TRUE, sizeof(glm::vec2), 0 }
    };
    vertex_array_.create_vertex_buffer<glm::vec2>(vertices, attributes, 0);
  }

  void set_uv(const std::span<const glm::vec2>& uv) override
  {
    std::vector<opengl::VertexAttribute> attributes{
      { 2, GL_FLOAT, GL_TRUE, sizeof(glm::vec2), 0 }
    };
    vertex_array_.create_vertex_buffer<glm::vec2>(uv, attributes, 1);
  }

  void set_indices(const std::span<const uint32_t>& indices) override
  {
    vertex_array_.create_element_buffer(indices);
  }

  void set_color(const glm::vec3& color) override
  {
    color_ = color;
  }

  void add_texture(interface::Texture::Type type, std::shared_ptr<interface::Texture> texture) override
  {
    texture_ = std::dynamic_pointer_cast<Texture>(texture);
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
