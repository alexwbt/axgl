#pragma once

#include <format>

#include <axgl/common.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/opengl/material.hpp>
#include <axgl/impl/opengl/renderer/render_context.hpp>
#include <axgl/impl/opengl/texture.hpp>

#include <opengl/static_shaders.hpp>

namespace axgl::impl::opengl
{

class BlinnPhongMaterial : public Material
{
  ::opengl::ShaderProgram& shader_ = ::opengl::StaticShaders::instance().blinn_phong();

  axgl::ptr_t<impl::opengl::Texture> diffuse_texture_;
  axgl::ptr_t<impl::opengl::Texture> specular_texture_;
  axgl::ptr_t<impl::opengl::Texture> normal_texture_;
  axgl::ptr_t<impl::opengl::Texture> height_texture_;
  float shininess_ = 32.0f;

public:
  void set_property(const std::string& key, const std::string& value) override
  {
    if (key == "shininess") shininess_ = std::stof(value);
#ifdef AXGL_DEBUG
    else AXGL_LOG_WARN("Property {} is not supported in phong material.", key);
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
    case kUnknown: AXGL_LOG_WARN("Adding texture with UNKNOWN type."); break;
#endif
    }
  }

  void use(const impl::opengl::renderer::RenderContext& context) override
  {
    Material::use(context);

    shader_.use_program();
    shader_.set_bool("transparent", enable_blend_);
    shader_.set_mat4("projection_view", context.projection_view_matrix);
    // shader_.set_mat4("light_pv", );
    shader_.set_vec3("camera_pos", context.viewpoint);
    shader_.set_vec4("mesh_color", color_);
    shader_.set_float("shininess", shininess_);
    shader_.set_float("alpha_discard", enable_blend_ ? 0.0f : alpha_discard_);
    shader_.set_vec2("uv_scale", uv_scale_);
    shader_.set_vec2("uv_offset", uv_offset_);
    shader_.set_float("diffuse_texture_gamma", 2.2f);

    use_lights(context.lights);

    use_texture(0, "diffuse", diffuse_texture_);
    use_texture(1, "specular", specular_texture_);
    use_texture(2, "normal", normal_texture_);
    use_texture(3, "height", height_texture_);
  }

private:
  void use_lights(const std::span<const impl::opengl::renderer::LightContext>& lights) const
  {
    int sun_lights_size = 0;
    int spot_lights_size = 0;
    int point_lights_size = 0;

    for (const auto& [light, light_pv] : lights)
    {
      switch (light->type)
      {
      case (axgl::Light::Type::kSun):
        shader_.set_vec3(std::format("sun_lights[{}].direction", sun_lights_size), light->direction);
        shader_.set_vec3(std::format("sun_lights[{}].ambient", sun_lights_size), light->color.ambient);
        shader_.set_vec3(std::format("sun_lights[{}].diffuse", sun_lights_size), light->color.diffuse);
        shader_.set_vec3(std::format("sun_lights[{}].specular", sun_lights_size), light->color.specular);
        sun_lights_size++;
        break;
      case (axgl::Light::Type::kPoint):
        shader_.set_vec3(std::format("point_lights[{}].position", point_lights_size), light->position);
        shader_.set_vec3(std::format("point_lights[{}].ambient", point_lights_size), light->color.ambient);
        shader_.set_vec3(std::format("point_lights[{}].diffuse", point_lights_size), light->color.diffuse);
        shader_.set_vec3(std::format("point_lights[{}].specular", point_lights_size), light->color.specular);
        shader_.set_float(std::format("point_lights[{}].constant", point_lights_size), light->strength.constant);
        shader_.set_float(std::format("point_lights[{}].linear", point_lights_size), light->strength.linear);
        shader_.set_float(std::format("point_lights[{}].quadratic", point_lights_size), light->strength.quadratic);
        point_lights_size++;
        break;
      case (axgl::Light::Type::kSpot):
        shader_.set_vec3(std::format("spot_lights[{}].direction", spot_lights_size), light->direction);
        shader_.set_vec3(std::format("spot_lights[{}].position", spot_lights_size), light->position);
        shader_.set_vec3(std::format("spot_lights[{}].ambient", spot_lights_size), light->color.ambient);
        shader_.set_vec3(std::format("spot_lights[{}].diffuse", spot_lights_size), light->color.diffuse);
        shader_.set_vec3(std::format("spot_lights[{}].specular", spot_lights_size), light->color.specular);
        shader_.set_float(std::format("spot_lights[{}].constant", spot_lights_size), light->strength.constant);
        shader_.set_float(std::format("spot_lights[{}].linear", spot_lights_size), light->strength.linear);
        shader_.set_float(std::format("spot_lights[{}].quadratic", spot_lights_size), light->strength.quadratic);
        shader_.set_float(std::format("spot_lights[{}].cut_off", spot_lights_size), light->cut_off);
        shader_.set_float(std::format("spot_lights[{}].outer_cut_off", spot_lights_size), light->outer_cut_off);
        spot_lights_size++;
        break;
      }
    }
    shader_.set_int("sun_lights_size", sun_lights_size);
    shader_.set_int("spot_lights_size", spot_lights_size);
    shader_.set_int("point_lights_size", point_lights_size);
  }

  void use_texture(
    const int texture_unit, const std::string& name, const axgl::ptr_t<impl::opengl::Texture>& texture) const
  {
    if (!texture) return;

    texture->use(GL_TEXTURE0 + texture_unit);
    shader_.set_int(name + "_texture", texture_unit);
    shader_.set_bool("use_" + name + "_texture", true);
  }
};

} // namespace axgl::impl::opengl
