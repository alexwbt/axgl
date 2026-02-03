#pragma once

#include <algorithm>
#include <array>

#include <axgl/common.hpp>
#include <axgl/interface/components/mesh.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/component_base.hpp>
#include <axgl/impl/opengl/material.hpp>
#include <axgl/impl/opengl/renderer.hpp>
#include <axgl/impl/opengl/renderer/render_component.hpp>

#include <opengl/static_shaders.hpp>
#include <opengl/vertex_array_object.hpp>

namespace axgl::impl::opengl::component
{

class Mesh : virtual public axgl::component::Mesh,
             virtual public axgl::impl::opengl::renderer::RenderComponent,
             public axgl::impl::ComponentBase
{
  axgl::ptr_t<axgl::impl::opengl::Material> material_;

  // mesh data stored in heap
  std::vector<glm::vec3> vertices_;
  std::vector<glm::vec2> vertices_2d_;
  std::vector<glm::vec3> normals_;
  std::vector<glm::vec2> uv_;
  std::vector<std::uint32_t> indices_;
  // mesh data stored in GPU memory
  std::unique_ptr<::opengl::VertexArrayObject> vao_;
  // instancing
  GLuint instanced_models_buffer_id_ = 0;
  std::vector<glm::mat4> instanced_models_;

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
  void set_indices(const std::span<const std::uint32_t>& indices) override
  {
    indices_.resize(indices.size());
    std::ranges::copy(indices, indices_.begin());
  }
  void set_material(const axgl::ptr_t<axgl::Material> material) override
  {
    material_ = std::dynamic_pointer_cast<impl::opengl::Material>(material);
#ifdef AXGL_DEBUG
    if (!material_) throw std::runtime_error("The provided material is not a valid opengl material.");
#endif
  }
  [[nodiscard]] axgl::ptr_t<axgl::Material> get_material() const override { return material_; }

  void gather_instances(const axgl::Entity& entity) override
  {
    if (!material_) return;
    instanced_models_.emplace_back(entity.get_model_matrix());
  }

  void submit_render_function(axgl::impl::opengl::renderer::PipelineContext& context) override
  {
    if (!material_)
    {
      AXGL_LOG_DEBUG("Material not assigned to mesh({}), skip rendering.", get_id());
      return;
    }

    if (!vao_) create_vao();
    vao_->set_mode(material_->draw_mode());

    if (instanced_models_buffer_id_ == 0)
    {
      constexpr size_t vec4_size = sizeof(glm::vec4);
      std::array attributes{
        ::opengl::VertexAttribute{4, GL_FLOAT, GL_FALSE, 4 * vec4_size, reinterpret_cast<void*>(0 * vec4_size)},
        ::opengl::VertexAttribute{4, GL_FLOAT, GL_FALSE, 4 * vec4_size, reinterpret_cast<void*>(1 * vec4_size)},
        ::opengl::VertexAttribute{4, GL_FLOAT, GL_FALSE, 4 * vec4_size, reinterpret_cast<void*>(2 * vec4_size)},
        ::opengl::VertexAttribute{4, GL_FLOAT, GL_FALSE, 4 * vec4_size, reinterpret_cast<void*>(3 * vec4_size)},
      };
      instanced_models_buffer_id_ = vao_->create_vertex_buffer<glm::mat4>(
        instanced_models_, attributes, material_->get_attribute_offset(axgl::impl::opengl::Material::kModels), 1);
    }
    else vao_->update_buffer_data<glm::mat4>(instanced_models_buffer_id_, instanced_models_);

    const auto instance_count = static_cast<GLsizei>(instanced_models_.size());
    instanced_models_.clear();

    const auto render_function = [this, instance_count](const axgl::impl::opengl::renderer::RenderContext& c)
    {
      material_->use(c);
      vao_->draw_instanced(instance_count);
    };
    if (material_->enabled_blend()) context.blend_pass.emplace_back(std::move(render_function));
    else
    {
      context.opaque_pass.emplace_back(std::move(render_function));
      context.shadow_pass.emplace_back([this, instance_count](const axgl::impl::opengl::renderer::LightContext& c)
      {
        // glEnable(GL_CULL_FACE);
        // glFrontFace(GL_CW);
        // glCullFace(GL_FRONT);

        auto& depth_only_shader = ::opengl::StaticShaders::instance().depth_only();
        depth_only_shader.use_program();
        depth_only_shader.set_mat4("projection_view", c.light_pv);
        vao_->draw_instanced(instance_count);
      });
    }
  }

  std::uint64_t get_id() override { return ComponentBase::get_id(); }

private:
  void create_vao()
  {
    vao_ = std::make_unique<::opengl::VertexArrayObject>();

    if (!vertices_.empty())
    {
      std::array attributes{::opengl::VertexAttribute{3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr}};
      vao_->create_vertex_buffer<glm::vec3>(
        vertices_, attributes, material_->get_attribute_offset(axgl::impl::opengl::Material::kVertices));
    }
    else if (!vertices_2d_.empty())
    {
      std::array attributes{::opengl::VertexAttribute{2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr}};
      vao_->create_vertex_buffer<glm::vec2>(
        vertices_2d_, attributes, material_->get_attribute_offset(axgl::impl::opengl::Material::kVertices));
    }

    if (!normals_.empty())
    {
      std::array attributes{::opengl::VertexAttribute{3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr}};
      vao_->create_vertex_buffer<glm::vec3>(
        normals_, attributes, material_->get_attribute_offset(axgl::impl::opengl::Material::kNormals));
    }

    if (!uv_.empty())
    {
      std::array attributes{::opengl::VertexAttribute{2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr}};
      vao_->create_vertex_buffer<glm::vec2>(
        uv_, attributes, material_->get_attribute_offset(axgl::impl::opengl::Material::kUV));
    }

    if (!indices_.empty()) vao_->create_element_buffer(indices_);
  }
};

} // namespace axgl::impl::opengl::component

