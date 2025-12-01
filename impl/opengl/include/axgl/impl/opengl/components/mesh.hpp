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
  axgl::ptr_t<impl::opengl::Material> material_;
  axgl::ptr_t<::opengl::VertexArrayObject> vertex_array_;

public:
  Mesh() { vertex_array_ = std::make_shared<::opengl::VertexArrayObject>(); }

  void render(const Entity::Context& context) override
  {
    if (material_)
    {
      // do not render and add sorted render if enabled blending
      // if (material_->enabled_blend())
      // {
      //   const auto camera = context.axgl.camera_service()->get_camera();
      //   const auto distance2 = glm::distance2(context.entity.transform().position, camera->position);
      //   sorted_render_service_->add_sorted_render(
      //     distance2, [this, &context]
      //   {
      //     this->render(context);
      //   });
      //   return;
      // }
      // use material and render
      material_->use(context, *this);
    }
    vertex_array_->draw();
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

  void set_material(const axgl::ptr_t<axgl::Material> material) override
  {
    material_ = std::dynamic_pointer_cast<impl::opengl::Material>(material);
#ifdef AXGL_DEBUG
    if (!material_)
      throw std::runtime_error("The provided material is not a valid opengl material.");
#endif
  }

  [[nodiscard]] axgl::ptr_t<axgl::Material> get_material() const override { return material_; }

  void replace_vao(axgl::ptr_t<::opengl::VertexArrayObject> vertex_array) { vertex_array_ = std::move(vertex_array); }
};

} // namespace axgl::impl::opengl::component

