#pragma once

#include <vector>
#include <stdexcept>

#include <axgl/axgl.hpp>
#include <axgl/namespace.hpp>
#include <axgl/interface/realm.hpp>
#include <axgl/interface/component/mesh.hpp>
#include <axgl/impl/opengl/opengl_material.hpp>

#include "opengl/vertex_array_object.hpp"

NAMESPACE_AXGL_IMPL

class OpenglMesh : public interface::Mesh
{
private:
  opengl::VertexArrayObject vertex_array_;
  std::shared_ptr<OpenglMaterial> material_;

public:
  void render() override
  {
    if (material_)
    {
      auto context = get_context();
      material_->use(context, this);
    }
    vertex_array_.draw();
  }

  void set_vertices(const std::span<const glm::vec3>& vertices) override
  {
    std::vector<opengl::VertexAttribute> attributes{
      { 3, GL_FLOAT, GL_TRUE, sizeof(glm::vec3), 0 }
    };
    vertex_array_.create_vertex_buffer<glm::vec3>(vertices, attributes, 0);
  }

  void set_vertices(const std::span<const glm::vec2>& vertices) override
  {
    std::vector<opengl::VertexAttribute> attributes{
      { 2, GL_FLOAT, GL_TRUE, sizeof(glm::vec2), 0 }
    };
    vertex_array_.create_vertex_buffer<glm::vec2>(vertices, attributes, 0);
  }

  void set_normals(const std::span<const glm::vec3>& normals) override
  {
    std::vector<opengl::VertexAttribute> attributes{
      { 3, GL_FLOAT, GL_TRUE, sizeof(glm::vec3), 0 }
    };
    vertex_array_.create_vertex_buffer<glm::vec3>(normals, attributes, 1);
  }

  void set_uv(const std::span<const glm::vec2>& uv) override
  {
    std::vector<opengl::VertexAttribute> attributes{
      { 2, GL_FLOAT, GL_TRUE, sizeof(glm::vec2), 0 }
    };
    vertex_array_.create_vertex_buffer<glm::vec2>(uv, attributes, 2);
  }

  void set_indices(const std::span<const uint32_t>& indices) override
  {
    vertex_array_.create_element_buffer(indices);
  }

  void set_material(std::shared_ptr<interface::Material> material) override
  {
    material_ = std::dynamic_pointer_cast<OpenglMaterial>(material);
#ifdef AXGL_DEBUG
    if (!material_)
      throw std::runtime_error("The provided material is not a valid opengl material.");
#endif
  }
};

NAMESPACE_AXGL_IMPL_END

NAMESPACE_AXGL

template<>
std::shared_ptr<interface::Mesh> interface::RealmService::create_component()
{
  return std::make_shared<impl::OpenglMesh>();
}

NAMESPACE_AXGL_END
