#pragma once

#include <array>
#include <memory>

#include <axgl/common.hpp>
#include <axgl/interface/realm.hpp>
#include <axgl/interface/component/mesh.hpp>

#include <axgl/impl/realm_service.hpp>
#include <axgl/impl/opengl/material.hpp>
#include <axgl/impl/opengl/renderer.hpp>

#include <opengl/vertex_array_object.hpp>

NAMESPACE_AXGL_IMPL namespace component
{
  class OpenglMesh : virtual public interface::component::Mesh, public ComponentBase
  {
    int attribute_offset_ = 0;
    std::shared_ptr<OpenglMaterial> material_;
    std::shared_ptr<opengl::VertexArrayObject> vertex_array_;

  public:
    OpenglMesh()
    {
      vertex_array_ = std::make_shared<opengl::VertexArrayObject>();
    }

    void render() override
    {
      ZoneScopedN("OpenglMesh Render");

      if (material_)
      {
        const auto context = get_context();
        const auto renderer = std::dynamic_pointer_cast<OpenglRenderer>(context->realm->get_renderer());
#ifdef AXGL_DEBUG
        if (!renderer)
          throw std::runtime_error("OpenglRenderer is required to render OpenglMesh.");
#endif
        const bool blend = material_->enabled_blend();
        const bool after = renderer->is_after_render();
        // do not render if blend is not enabled and is after render
        if (!blend && after)
          return;
        // do not render and add to blend render list if enabled blending
        if (blend && !after)
        {
          const auto parent = get_parent();
          const auto distance2 = glm::distance2(parent->transform()->position, context->camera->position);
          renderer->add_blend_render(distance2, parent);
          return;
        }
        // use material and render
        material_->use(context, this);
      }
      vertex_array_->draw();
    }

    void set_vertices(const std::span<const glm::vec3>& vertices) override
    {
      std::array attributes{ opengl::VertexAttribute{ 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0 } };
      vertex_array_->create_vertex_buffer<glm::vec3>(vertices, attributes, attribute_offset_++);
    }

    void set_vertices(const std::span<const glm::vec2>& vertices) override
    {
      std::array attributes{ opengl::VertexAttribute{ 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0 } };
      vertex_array_->create_vertex_buffer<glm::vec2>(vertices, attributes, attribute_offset_++);
    }

    void set_normals(const std::span<const glm::vec3>& normals) override
    {
      std::array attributes{ opengl::VertexAttribute{ 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0 } };
      vertex_array_->create_vertex_buffer<glm::vec3>(normals, attributes, attribute_offset_++);
    }

    void set_uv(const std::span<const glm::vec2>& uv) override
    {
      std::array attributes{ opengl::VertexAttribute{ 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0 } };
      vertex_array_->create_vertex_buffer<glm::vec2>(uv, attributes, attribute_offset_++);
    }

    void set_indices(const std::span<const uint32_t>& indices) override
    {
      vertex_array_->create_element_buffer(indices);
    }

    void set_material(const std::shared_ptr<interface::Material> material) override
    {
      material_ = std::dynamic_pointer_cast<OpenglMaterial>(material);
#ifdef AXGL_DEBUG
      if (!material_)
        throw std::runtime_error("The provided material is not a valid opengl material.");
#endif
    }

    [[nodiscard]] std::shared_ptr<interface::Material> get_material() const override
    {
      return material_;
    }

    void replace_vao(std::shared_ptr<opengl::VertexArrayObject> vertex_array)
    {
      vertex_array_ = std::move(vertex_array);
    }
  };
}

NAMESPACE_AXGL_IMPL_END
