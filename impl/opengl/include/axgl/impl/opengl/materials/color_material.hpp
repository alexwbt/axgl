#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/opengl/material.hpp>

#include <opengl/static_shaders.hpp>

namespace axgl::impl::opengl
{

class ColorMaterial : public Material
{
  ::opengl::ShaderProgram& shader_ = ::opengl::StaticShaders::instance().color();

public:
  void use(const RenderComponent::Context& context) override
  {
    Material::use(context);
    shader_.use_program();
    shader_.set_vec4("color", color_);
  }
};

} // namespace axgl::impl::opengl
