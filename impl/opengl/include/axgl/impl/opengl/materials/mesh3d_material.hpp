#pragma once

#include <format>

#include <axgl/common.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/opengl/material.hpp>
#include <axgl/impl/opengl/renderer/constants.hpp>
#include <axgl/impl/opengl/renderer/render_context.hpp>
#include <axgl/impl/opengl/renderer/sun_shadow_map.hpp>
#include <axgl/impl/opengl/shaders.hpp>
#include <axgl/impl/opengl/texture.hpp>

namespace axgl::impl::opengl
{

class Mesh3dMaterial : public Material
{
  axgl::ptr_t<impl::opengl::Texture> diffuse_texture_;
  axgl::ptr_t<impl::opengl::Texture> specular_texture_;
  axgl::ptr_t<impl::opengl::Texture> normal_texture_;
  axgl::ptr_t<impl::opengl::Texture> height_texture_;
  float shininess_ = 32.0f;
  float specular_ = 1.0f;

public:
  void set_property(const std::string& key, const std::string& value) override
  {
    if (key == "shininess") shininess_ = std::stof(value);
    else if (key == "specular") specular_ = std::stof(value);
#ifdef AXGL_DEBUG
    else AXGL_LOG_WARN("Property {} is not supported in mesh3d material.", key);
#endif
  }

  void add_texture(
    const axgl::Material::TextureType type,
    const axgl::ptr_t<axgl::Texture> texture
  ) override
  {
    auto texture_ = std::dynamic_pointer_cast<impl::opengl::Texture>(texture);
    if (!texture_)
#ifdef AXGL_DEBUG
      throw std::runtime_error(
        "The provided texture is not a valid opengl texture."
      );
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

  void use(const renderer::RenderContext& context) override
  {
    Material::use(context);

    const auto& shader = *get_shader();
    shader.use_program();
    shader.set_bool("transparent", enable_blend_);
    shader.set_mat4("projection_view", context.projection_view_matrix);
    shader.set_vec3("camera_pos", context.viewpoint);
    shader.set_vec2("viewport", context.viewport);
    shader.set_vec4("mesh_color", color_);
    shader.set_float("mesh_shininess", shininess_);
    shader.set_float("mesh_specular", specular_);
    shader.set_float("alpha_discard", enable_blend_ ? 0.0f : alpha_discard_);
    shader.set_vec2("uv_scale", uv_scale_);
    shader.set_vec2("uv_offset", uv_offset_);
    shader.set_float("diffuse_texture_gamma", 2.2f);
    shader.set_float("height_scale", height_scale_);
    shader.set_float("normal_scale", normal_scale_);

    use_sun_lights(shader, context);
    use_spot_lights(shader, context);
    use_point_lights(shader, context);

    use_texture(shader, 0, "diffuse", diffuse_texture_);
    use_texture(shader, 1, "specular", specular_texture_);
    use_texture(shader, 2, "normal", normal_texture_);
    use_texture(shader, 3, "height", height_texture_);

    const auto use_ssao = context.ssao_texture != nullptr;
    shader.set_bool("use_ssao", use_ssao);
    if (use_ssao)
    {
      context.ssao_texture->use(GL_TEXTURE4);
      shader.set_int("ssao_texture", 4);
    }

    const auto use_sun_shadow = context.sun_shadow_maps != nullptr;
    shader.set_bool("enable_sun_shadow", use_sun_shadow);
#ifdef AXGL_DEBUG
    shader.set_bool("csm_debug_borders", context.csm_debug_borders);
#endif
    if (use_sun_shadow)
    {
      context.sun_shadow_maps->use(GL_TEXTURE5);
      shader.set_int("sun_shadow_maps", 5);
    }

    const auto use_spot_shadow = context.spot_shadow_maps != nullptr;
    shader.set_bool("enable_spot_shadow", use_spot_shadow);
    if (use_spot_shadow)
    {
      context.spot_shadow_maps->use(GL_TEXTURE6);
      shader.set_int("spot_shadow_maps", 6);
    }
  }

  [[nodiscard]] const ::opengl::ShaderProgram* get_shader() const override
  {
    return enable_blend_ ? &Shaders::instance().mesh3d()
                         : &Shaders::instance().mesh3d_opaque();
  }

private:
  void use_sun_lights(
    const ::opengl::ShaderProgram& shader,
    const renderer::RenderContext& render_context
  ) const
  {
    constexpr auto cascade_count
      = static_cast<GLsizei>(renderer::kSunShadowCascadeCount);

    int size = static_cast<int>(
      std::min(render_context.sun_lights.size(), renderer::kSunLightLimit)
    );
    shader.set_int("sun_lights_size", size);

    std::size_t sun_shadow_count = 0;
    for (int i = 0; i < size; ++i)
    {
      const auto& context = render_context.sun_lights[i];
      const auto* light = context.light;

      shader.set_vec3(
        std::format("sun_lights[{}].direction", i), light->direction
      );
      shader.set_vec3(
        std::format("sun_lights[{}].ambient", i), light->color.ambient
      );
      shader.set_vec3(
        std::format("sun_lights[{}].diffuse", i), light->color.diffuse
      );
      shader.set_vec3(
        std::format("sun_lights[{}].specular", i), light->color.specular
      );

      if (light->casts_shadows
          && sun_shadow_count++ < renderer::kSunShadowLimit)
      {
        // upload the per-cascade light PVs + split distances and bind the
        // sampler2DArray; the FS selects the cascade by fragment distance.
        std::array<glm::mat4, cascade_count> cascade_pvs{};
        std::array<float, cascade_count> cascade_far{};
        for (std::size_t j = 0; j < cascade_count; ++j)
        {
          cascade_pvs[j] = context.cascades[j].light_pv;
          cascade_far[j] = context.cascades[j].split_far;
        }
        shader.set_mat4_array(
          "cascade_light_pv", cascade_count, cascade_pvs.data()
        );
        shader.set_float_array(
          "cascade_split_far", cascade_count, cascade_far.data()
        );
      }
    }
  }

  void use_point_lights(
    const ::opengl::ShaderProgram& shader,
    const renderer::RenderContext& render_context
  ) const
  {
    int size = static_cast<int>(
      std::min(render_context.point_lights.size(), renderer::kPointLightLimit)
    );
    shader.set_int("point_lights_size", size);

    for (int i = 0; i < size; ++i)
    {
      const auto& context = render_context.point_lights[i];
      const auto* light = context.light;

      shader.set_vec3(
        std::format("point_lights[{}].position", i), light->position
      );
      shader.set_vec3(
        std::format("point_lights[{}].ambient", i), light->color.ambient
      );
      shader.set_vec3(
        std::format("point_lights[{}].diffuse", i), light->color.diffuse
      );
      shader.set_vec3(
        std::format("point_lights[{}].specular", i), light->color.specular
      );
      shader.set_float(
        std::format("point_lights[{}].constant", i), light->strength.constant
      );
      shader.set_float(
        std::format("point_lights[{}].linear", i), light->strength.linear
      );
      shader.set_float(
        std::format("point_lights[{}].quadratic", i), light->strength.quadratic
      );
    }
  }

  void use_spot_lights(
    const ::opengl::ShaderProgram& shader,
    const renderer::RenderContext& render_context
  ) const
  {
    int size = static_cast<int>(
      std::min(render_context.spot_lights.size(), renderer::kSpotLightLimit)
    );
    shader.set_int("spot_lights_size", size);

    std::size_t spot_shadow_count = 0;
    for (int i = 0; i < size; ++i)
    {
      const auto& context = render_context.spot_lights[i];
      const auto* light = context.light;

      shader.set_vec3(
        std::format("spot_lights[{}].direction", i), light->direction
      );
      shader.set_vec3(
        std::format("spot_lights[{}].position", i), light->position
      );
      shader.set_vec3(
        std::format("spot_lights[{}].ambient", i), light->color.ambient
      );
      shader.set_vec3(
        std::format("spot_lights[{}].diffuse", i), light->color.diffuse
      );
      shader.set_vec3(
        std::format("spot_lights[{}].specular", i), light->color.specular
      );
      shader.set_float(
        std::format("spot_lights[{}].constant", i), light->strength.constant
      );
      shader.set_float(
        std::format("spot_lights[{}].linear", i), light->strength.linear
      );
      shader.set_float(
        std::format("spot_lights[{}].quadratic", i), light->strength.quadratic
      );
      shader.set_float(
        std::format("spot_lights[{}].cut_off", i), light->cut_off
      );
      shader.set_float(
        std::format("spot_lights[{}].outer_cut_off", i), light->outer_cut_off
      );

      if (light->casts_shadows && context.shadow_index >= 0
          && spot_shadow_count++ < renderer::kSpotShadowLimit)
      {
        shader.set_int(
          std::format("spot_shadow_index[{}]", i), context.shadow_index
        );
        shader.set_mat4(
          std::format("spot_shadow_pv[{}]", context.shadow_index),
          context.light_pv
        );
      }
      else shader.set_int(std::format("spot_shadow_index[{}]", i), -1);
    }
  }

  void use_texture(
    const ::opengl::ShaderProgram& shader,
    const int texture_unit,
    const std::string& name,
    const axgl::ptr_t<impl::opengl::Texture>& texture
  ) const
  {
    if (!texture)
    {
      shader.set_bool(std::format("use_{}_texture", name), false);
      return;
    }

    texture->use(GL_TEXTURE0 + texture_unit);
    shader.set_bool(std::format("use_{}_texture", name), true);
    shader.set_int(std::format("{}_texture", name), texture_unit);
  }
};

} // namespace axgl::impl::opengl
