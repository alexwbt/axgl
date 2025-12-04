#pragma once

#include <axgl/common.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/opengl/material.hpp>
#include <axgl/impl/opengl/texture.hpp>

namespace axgl::impl::opengl
{

class Default2DMaterial : public Material
{
  ::opengl::ShaderProgram& shader_ = ::opengl::StaticShaders::instance().mesh_2d();
  axgl::ptr_t<impl::opengl::Texture> texture_;

public:
  void set_prop(const std::string& key, const std::string& value) override { }

  void add_texture(axgl::Material::TextureType type, axgl::ptr_t<axgl::Texture> texture) override
  {
    texture_ = std::dynamic_pointer_cast<Texture>(texture);
#ifdef AXGL_DEBUG
    if (!texture_)
      throw std::runtime_error("The provided texture is not a valid opengl texture.");
#endif
  }

  void use(const RenderComponent::Context& context) override
  {
    Material::use(context);

    shader_.use_program();
    shader_.set_bool("use_instancing", true);
    shader_.set_mat4("projection_view", context.camera->projection_view_matrix());
    shader_.set_vec4("mesh_color", color_);

    if (texture_)
    {
      glActiveTexture(GL_TEXTURE0);
      texture_->use();
      shader_.set_int("mesh_texture", 0);
      shader_.set_bool("use_texture", true);
    }
    else
    {
      shader_.set_bool("use_texture", false);
    }
  }

  [[nodiscard]] int get_attribute_offset(const Attribute attribute) const override
  {
    switch (attribute)
    {
    case kVertices: return 0;
    case kUv: return 1;
    case kModels: return 2;
    default: return 3;
    }
  }
};

} // namespace axgl::impl::opengl
