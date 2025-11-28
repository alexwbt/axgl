#pragma once

#ifdef AXGL_DEBUG
#include <stdexcept>
#endif
#include <memory>
#include <string>

#include <axgl/axgl.hpp>
#include <axgl/interface/components/mesh.hpp>
#include <axgl/interface/realm.hpp>

#include <axgl/impl/opengl/material.hpp>
#include <axgl/impl/opengl/texture.hpp>

namespace axgl::impl::opengl
{

class Default2DMaterial : public Material
{
  std::shared_ptr<::opengl::ShaderProgram> shader_ = ::opengl::StaticShaders::instance().mesh_2d();
  std::shared_ptr<impl::opengl::Texture> texture_;

public:
  void set_prop(const std::string& key, const std::string& value) override { }

  void add_texture(axgl::Material::TextureType type, std::shared_ptr<axgl::Texture> texture) override
  {
    texture_ = std::dynamic_pointer_cast<Texture>(texture);
#ifdef AXGL_DEBUG
    if (!texture_)
      throw std::runtime_error("The provided texture is not a valid opengl texture.");
#endif
  }

  void use(const axgl::Realm::Context* context, const axgl::component::Mesh* mesh) override
  {
    Material::use(context, mesh);

    const auto camera = context->axgl->camera_service()->get_camera();
    const auto model = mesh->get_entity()->get_model_matrix();
    const auto mvp = camera->projection_view_matrix() * model;

    shader_->use_program();
    shader_->set_mat4("mvp", mvp);
    shader_->set_vec4("mesh_color", color_);

    if (texture_)
    {
      glActiveTexture(GL_TEXTURE0);
      texture_->use();
      shader_->set_int("mesh_texture", 0);
      shader_->set_bool("use_texture", true);
    }
    else
    {
      shader_->set_bool("use_texture", false);
    }
  }
};

} // namespace axgl::impl::opengl
