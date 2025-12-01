#pragma once

#include <array>
#include <memory>
#include <span>

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include <axgl/interface/components/mesh.hpp>
#include <axgl/interface/renderer.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/component_base.hpp>
#include <axgl/impl/opengl/material.hpp>
#include <axgl/impl/opengl/renderer.hpp>

#include <opengl/vertex_array_object.hpp>

namespace axgl::impl::opengl::component
{

class Mesh : virtual public axgl::component::Mesh, public ComponentBase
{
  int attribute_offset_ = 0;
  std::shared_ptr<impl::opengl::Material> material_;
  std::shared_ptr<::opengl::VertexArrayObject> vertex_array_;

  struct Renderable : Renderer::Renderable
  {
    Axgl* axgl;
    Realm* realm;
    Entity* entity;
    Mesh* mesh;
    void render() const override
    {
      if (mesh->material_)
      {
        // use material and render
        mesh->material_->use(context, *this);
      }
      mesh->vertex_array_->draw();
    }
  };
  Renderable renderable_;

public:
  Mesh() { vertex_array_ = std::make_shared<::opengl::VertexArrayObject>(); }

  void render(const Entity::Context& context) override
  {
    renderable_.axgl = &context.axgl;
    renderable_.realm = &context.realm;
    renderable_.entity = &context.entity;
    renderable_.mesh = this;
    context.realm.get_renderer()->add_renderable(&renderable_);
  }

  void set_vertices(const std::span<const glm::vec3>& vertices) override
  {
    std::array attributes{::opengl::VertexAttribute{3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0}};
    vertex_array_->create_vertex_buffer<glm::vec3>(vertices, attributes, attribute_offset_++);
  }

  void set_vertices(const std::span<const glm::vec2>& vertices) override
  {
    std::array attributes{::opengl::VertexAttribute{2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0}};
    vertex_array_->create_vertex_buffer<glm::vec2>(vertices, attributes, attribute_offset_++);
  }

  void set_normals(const std::span<const glm::vec3>& normals) override
  {
    std::array attributes{::opengl::VertexAttribute{3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0}};
    vertex_array_->create_vertex_buffer<glm::vec3>(normals, attributes, attribute_offset_++);
  }

  void set_uv(const std::span<const glm::vec2>& uv) override
  {
    std::array attributes{::opengl::VertexAttribute{2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0}};
    vertex_array_->create_vertex_buffer<glm::vec2>(uv, attributes, attribute_offset_++);
  }

  void set_indices(const std::span<const uint32_t>& indices) override { vertex_array_->create_element_buffer(indices); }

  void set_material(const std::shared_ptr<axgl::Material> material) override
  {
    material_ = std::dynamic_pointer_cast<impl::opengl::Material>(material);
#ifdef AXGL_DEBUG
    if (!material_)
      throw std::runtime_error("The provided material is not a valid opengl material.");
#endif
  }

  [[nodiscard]] std::shared_ptr<axgl::Material> get_material() const override { return material_; }

  void replace_vao(std::shared_ptr<::opengl::VertexArrayObject> vertex_array)
  {
    vertex_array_ = std::move(vertex_array);
  }
};

} // namespace axgl::impl::opengl::component

