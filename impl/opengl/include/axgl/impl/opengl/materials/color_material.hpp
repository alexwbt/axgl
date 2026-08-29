#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/opengl/material.hpp>
#include <axgl/impl/opengl/renderer/render_context.hpp>
#include <axgl/impl/opengl/shaders.hpp>

namespace axgl::impl::opengl
{

class ColorMaterial : public Material
{
  const ::opengl::ShaderProgram& shader_ = Shaders::instance().color();

public:
  void use(const renderer::RenderContext& context) override
  {
    Material::use(context);
    shader_.use_program();

    shader_.set_mat4("projection_view", context.projection_view_matrix);
    shader_.set_vec4("color", color_);
  }

  [[nodiscard]] const ::opengl::ShaderProgram* get_shader() const override
  {
    return &shader_;
  }
};

} // namespace axgl::impl::opengl
