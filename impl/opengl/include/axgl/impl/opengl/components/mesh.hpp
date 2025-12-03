#pragma once

#include <algorithm>
#include <array>
#include <vector>

#include <axgl/common.hpp>
#include <axgl/interface/components/mesh.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/component_base.hpp>
#include <axgl/impl/opengl/material.hpp>
#include <axgl/impl/opengl/renderable_component.hpp>
#include <axgl/impl/opengl/renderer.hpp>

#include <opengl/vertex_array_object.hpp>

namespace axgl::impl::opengl::component
{

class Mesh : virtual public axgl::component::Mesh,
             virtual public axgl::impl::opengl::RenderableComponent,
             public axgl::impl::ComponentBase
{
  axgl::ptr_t<axgl::impl::opengl::Material> material_;
  axgl::ptr_t<::opengl::VertexArrayObject> vertex_array_;

  std::vector<glm::vec3> vertices_;
  std::vector<glm::vec2> vertices_2d_;
  std::vector<glm::vec3> normals_;
  std::vector<glm::vec2> uv_;
  std::vector<glm::mat4> model_matrices_;

public:
  void set_vertices(const std::span<const glm::vec3>& vertices) override
  {
    vertices_.resize(vertices.size());
    std::ranges::copy(vertices, vertices_.begin());
  }
  void set_vertices(const std::span<const glm::vec2>& vertices) override
  {
    vertices_2d_.resize(vertices.size());
    std::ranges::copy(vertices, vertices_2d_.begin());
  }
  void set_normals(const std::span<const glm::vec3>& normals) override
  {
    normals_.resize(normals.size());
    std::ranges::copy(normals, normals_.begin());
  }
  void set_uv(const std::span<const glm::vec2>& uv) override
  {
    uv_.resize(uv.size());
    std::ranges::copy(uv, uv_.begin());
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

  void collect_renderable(const axgl::Entity& entity) override
  {
    // if (material_)
    // {
    //   // TODO: Handle blending
    //   // TODO: And instancing (time to beat 10ms)
    //   // use material and render
    //   material_->use({entity, &context.camera, context.lights});
    // }
    // vertex_array_->draw();
  }

  void build_renderable(const RenderableComponent::Context& context) override
  {
    // if (material_)
    // {
    //   // TODO: Handle blending
    //   // TODO: And instancing (time to beat 10ms)
    //   // use material and render
    //   material_->use({entity, &context.camera, context.lights});
    // }
    // vertex_array_->draw();
  }

  bool enabled_blend() override { return material_ && material_->enabled_blend(); }

private:
  void set_model_matrices(const std::span<const glm::mat4>& model_matrices)
  {
    model_matrices_.resize(model_matrices.size());
    std::ranges::copy(model_matrices, model_matrices_.begin());
  }

  void create_vertices_buffer()
  {
    if (!vertex_array_ || !material_ || vertices_.empty())
      return;
    std::array attributes{::opengl::VertexAttribute{3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr}};
    vertex_array_->create_vertex_buffer<glm::vec3>(
      vertices_, attributes, material_->get_attribute_offset(axgl::impl::opengl::Material::kVertices));
  }

  void create_vertices_2d_buffer()
  {
    if (!vertex_array_ || !material_ || vertices_2d_.empty())
      return;
    std::array attributes{::opengl::VertexAttribute{2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr}};
    vertex_array_->create_vertex_buffer<glm::vec2>(
      vertices_2d_, attributes, material_->get_attribute_offset(axgl::impl::opengl::Material::kVertices));
  }

  void create_normals_buffer()
  {
    if (!vertex_array_ || !material_ || normals_.empty())
      return;
    std::array attributes{::opengl::VertexAttribute{3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr}};
    vertex_array_->create_vertex_buffer<glm::vec3>(
      normals_, attributes, material_->get_attribute_offset(axgl::impl::opengl::Material::kNormals));
  }

  void create_uv_buffer()
  {
    if (!vertex_array_ || !material_ || uv_.empty())
      return;
    std::array attributes{::opengl::VertexAttribute{2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr}};
    vertex_array_->create_vertex_buffer<glm::vec2>(
      uv_, attributes, material_->get_attribute_offset(axgl::impl::opengl::Material::kUv));
  }

  void create_model_matrices_buffer()
  {
    if (!vertex_array_ || !material_ || model_matrices_.empty())
      return;
    constexpr size_t vec4_size = sizeof(glm::vec4);
    std::array attributes{
      ::opengl::VertexAttribute{4, GL_FLOAT, GL_FALSE, vec4_size, reinterpret_cast<void*>(0 * vec4_size)},
      ::opengl::VertexAttribute{4, GL_FLOAT, GL_FALSE, vec4_size, reinterpret_cast<void*>(1 * vec4_size)},
      ::opengl::VertexAttribute{4, GL_FLOAT, GL_FALSE, vec4_size, reinterpret_cast<void*>(2 * vec4_size)},
      ::opengl::VertexAttribute{4, GL_FLOAT, GL_FALSE, vec4_size, reinterpret_cast<void*>(3 * vec4_size)},
    };
    vertex_array_->create_vertex_buffer<glm::mat4>(
      model_matrices_, attributes, material_->get_attribute_offset(axgl::impl::opengl::Material::kModels));
  }

  void create_vao_buffer()
  {
    vertex_array_ = axgl::create_ptr<::opengl::VertexArrayObject>();
    create_vertices_buffer();
    create_vertices_2d_buffer();
    create_normals_buffer();
    create_uv_buffer();
    create_model_matrices_buffer();
  }
};

} // namespace axgl::impl::opengl::component

