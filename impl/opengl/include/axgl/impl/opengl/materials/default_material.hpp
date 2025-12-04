#pragma once

#include <axgl/common.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/opengl/material.hpp>
#include <axgl/impl/opengl/texture.hpp>

namespace axgl::impl::opengl
{

class DefaultMaterial : public Material
{
  ::opengl::ShaderProgram& shader_ = ::opengl::StaticShaders::instance().mesh_3d();
  float shininess_ = 1.0f;

  axgl::ptr_t<impl::opengl::Texture> diffuse_texture_;
  axgl::ptr_t<impl::opengl::Texture> specular_texture_;
  axgl::ptr_t<impl::opengl::Texture> normal_texture_;
  axgl::ptr_t<impl::opengl::Texture> height_texture_;

public:
  void set_prop(const std::string& key, const std::string& value) override
  {
    if (key == "shininess")
      shininess_ = std::stof(value);
#ifdef AXGL_DEBUG
    else
      throw std::runtime_error("Invalid material prop key: " + key);
#endif
  }

  void add_texture(const axgl::Material::TextureType type, const axgl::ptr_t<axgl::Texture> texture) override
  {
    auto texture_ = std::dynamic_pointer_cast<impl::opengl::Texture>(texture);
    if (!texture_)
#ifdef AXGL_DEBUG
      throw std::runtime_error("The provided texture is not a valid opengl texture.");
#else
      return;
#endif
    using enum axgl::Material::TextureType;
    switch (type)
    {
    case kDiffuse: diffuse_texture_ = std::move(texture_); break;
    case kSpecular: specular_texture_ = std::move(texture_); break;
    case kNormal: normal_texture_ = std::move(texture_); break;
    case kHeight: height_texture_ = std::move(texture_); break;
#ifdef AXGL_DEBUG
    case kUnknown: SPDLOG_WARN("Adding texture with UNKNOWN type."); break;
#endif
    }
  }

  void use(const RenderComponent::Context& context) override
  {
    Material::use(context);

    shader_.use_program();
    shader_.set_bool("use_instancing", true);
    shader_.set_mat4("projection_view", context.camera->projection_view_matrix());
    shader_.set_vec3("camera_pos", context.camera->position);
    shader_.set_vec4("mesh_color", color_);
    shader_.set_float("mesh_shininess", shininess_);

    use_lights(context.lights);

    use_texture(0, "diffuse", diffuse_texture_);
    use_texture(1, "specular", specular_texture_);
    use_texture(2, "normal", normal_texture_);
    use_texture(3, "height", height_texture_);
  }

  [[nodiscard]] int get_attribute_offset(const Attribute attribute) const override
  {
    switch (attribute)
    {
    case kVertices: return 0;
    case kNormals: return 1;
    case kUv: return 2;
    case kModels: return 3;
    default: return 4;
    }
  }

private:
  void use_lights(const std::span<const axgl::Light* const>& lights) const
  {
    int sun_lights_size = 0;
    int spot_lights_size = 0;
    int point_lights_size = 0;

    for (const auto& light : lights)
    {
      switch (light->type)
      {
      case (axgl::Light::kSun):
        shader_.set_vec3("sun_lights[" + std::to_string(sun_lights_size) + "].direction", light->direction);
        shader_.set_vec3("sun_lights[" + std::to_string(sun_lights_size) + "].ambient", light->color.ambient);
        shader_.set_vec3("sun_lights[" + std::to_string(sun_lights_size) + "].diffuse", light->color.diffuse);
        shader_.set_vec3("sun_lights[" + std::to_string(sun_lights_size) + "].specular", light->color.specular);
        sun_lights_size++;
        break;
      case (axgl::Light::kPoint):
        shader_.set_vec3("point_lights[" + std::to_string(point_lights_size) + "].position", light->position);
        shader_.set_vec3("point_lights[" + std::to_string(point_lights_size) + "].ambient", light->color.ambient);
        shader_.set_vec3("point_lights[" + std::to_string(point_lights_size) + "].diffuse", light->color.diffuse);
        shader_.set_vec3("point_lights[" + std::to_string(point_lights_size) + "].specular", light->color.specular);
        shader_.set_float("point_lights[" + std::to_string(point_lights_size) + "].constant", light->strength.constant);
        shader_.set_float("point_lights[" + std::to_string(point_lights_size) + "].linear", light->strength.linear);
        shader_.set_float(
          "point_lights[" + std::to_string(point_lights_size) + "].quadratic", light->strength.quadratic);
        point_lights_size++;
        break;
      case (axgl::Light::kSpot):
        shader_.set_vec3("spot_lights[" + std::to_string(spot_lights_size) + "].direction", light->direction);
        shader_.set_vec3("spot_lights[" + std::to_string(spot_lights_size) + "].position", light->position);
        shader_.set_vec3("spot_lights[" + std::to_string(spot_lights_size) + "].ambient", light->color.ambient);
        shader_.set_vec3("spot_lights[" + std::to_string(spot_lights_size) + "].diffuse", light->color.diffuse);
        shader_.set_vec3("spot_lights[" + std::to_string(spot_lights_size) + "].specular", light->color.specular);
        shader_.set_float("spot_lights[" + std::to_string(spot_lights_size) + "].constant", light->strength.constant);
        shader_.set_float("spot_lights[" + std::to_string(spot_lights_size) + "].linear", light->strength.linear);
        shader_.set_float("spot_lights[" + std::to_string(spot_lights_size) + "].quadratic", light->strength.quadratic);
        shader_.set_float("spot_lights[" + std::to_string(spot_lights_size) + "].cutOff", light->cut_off);
        shader_.set_float("spot_lights[" + std::to_string(spot_lights_size) + "].outerCutOff", light->outer_cut_off);
        spot_lights_size++;
        break;
      }
    }
    shader_.set_int("sun_lights_size", sun_lights_size);
    shader_.set_int("spot_lights_size", spot_lights_size);
    shader_.set_int("point_lights_size", point_lights_size);
  }

  void use_texture(const int i, const std::string& name, const axgl::ptr_t<impl::opengl::Texture>& texture) const
  {
    if (!texture)
      return;

    glActiveTexture(GL_TEXTURE0 + i);
    texture->use();
    shader_.set_int(name + "_texture", i);
    shader_.set_bool("use_" + name + "_texture", true);
  }
};

} // namespace axgl::impl::opengl
