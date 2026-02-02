#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/opengl/material.hpp>
#include <axgl/impl/opengl/renderer/render_context.hpp>

#include <opengl/static_shaders.hpp>

namespace axgl::impl::opengl
{

class ColorMaterial : public Material
{
  ::opengl::ShaderProgram& shader_ = ::opengl::StaticShaders::instance().color();

public:
  void use(const impl::opengl::renderer::RenderContext& context) override
  {
    Material::use(context);
    shader_.use_program();

    shader_.set_mat4("projection_view", context.projection_view_matrix);
    shader_.set_vec4("color", color_);
  }
};

} // namespace axgl::impl::opengl
