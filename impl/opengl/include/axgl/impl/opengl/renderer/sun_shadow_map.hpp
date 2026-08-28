#pragma once

#include <array>
#include <limits>
#include <memory>
#include <source_location>

#include <axgl/common.hpp>
#include <axgl/interface/light.hpp>

#include <axgl/impl/opengl/renderer/constants.hpp>
#include <axgl/impl/opengl/renderer/render_context.hpp>
#include <axgl/impl/opengl/renderer/sun_shadow_cascade.hpp>

#include <opengl/framebuffer.hpp>
#include <opengl/texture.hpp>

namespace axgl::impl::opengl::renderer
{

struct SunShadowMap
{
  static constexpr GLsizei cascade_count
    = static_cast<GLsizei>(kSunShadowCascadeCount);

  std::unique_ptr<::opengl::Texture> shadow_texture;
  std::unique_ptr<::opengl::Framebuffer> shadow_framebuffer;

  void setup(const GLsizei shadow_map_size)
  {
    // one 2D-array texture layer per cascade; the FBO attaches a single layer
    // at a time in the render loop below.
    shadow_texture = std::make_unique<::opengl::Texture>();
    shadow_texture->load_texture_array(
      0, GL_DEPTH_COMPONENT, shadow_map_size, shadow_map_size, cascade_count, 0,
      GL_DEPTH_COMPONENT, GL_FLOAT, nullptr
    );
    shadow_texture->set_parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    shadow_texture->set_parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    shadow_texture->set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    shadow_texture->set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    shadow_texture->set_parameter(
      GL_TEXTURE_BORDER_COLOR, std::array{1.0f, 1.0f, 1.0f, 1.0f}
    );
    shadow_texture->set_parameter(GL_TEXTURE_COMPARE_MODE, GL_NONE);
    shadow_framebuffer = std::make_unique<::opengl::Framebuffer>();
  }

  void reset()
  {
    shadow_framebuffer.reset();
    shadow_texture.reset();
  }

  void render(
    const axgl::Light& light,
    const RenderContext& render_context,
    const PipelineContext& pipeline_context,
    const GLsizei shadow_map_size,
    const float shadow_distance,
    SunLightContext& output
  )
  {
    output.cascades = SunShadowCascade::get_cascades(
      light, render_context.inverse_projection_view_matrix,
      render_context.camera_near, render_context.camera_far, shadow_distance
    );
    output.shadow_map = shadow_texture.get();

    glViewport(0, 0, shadow_map_size, shadow_map_size);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthRange(0.0f, 1.0f);

    AXGL_PROFILE_SCOPE("Render Shadow Map");
    // Render the scene once per cascade: attach layer c, set its light_pv,
    // clear, draw. The depth_only shader reads light_pv from the LightContext
    // so no shadow-pass shader changes are needed.
    for (GLsizei i = 0; i < cascade_count; ++i)
    {
      shadow_framebuffer->use();
      shadow_framebuffer->attach_texture_layer(
        GL_DEPTH_ATTACHMENT, *shadow_texture, i
      );
      shadow_framebuffer->check_status_complete();
      glClearDepth(1.0);
      glClear(GL_DEPTH_BUFFER_BIT);

      ShadowPassContext shadow_pass_context{
        .projection_view_matrix = output.cascades[i].light_pv
      };
      for (const auto& render_func : pipeline_context.shadow_pass)
        render_func(shadow_pass_context);
    }
  }
};

} // namespace axgl::impl::opengl::renderer
