#pragma once

#include <axgl/impl/opengl/renderer/sun_shadow_map.hpp>
#include <glad/glad.h>

namespace axgl::impl::opengl::renderer
{

struct Shadow
{
  bool enabled = false;
  bool enable_csm_debug = false;
  float shadow_distance = 100.0f;
  GLsizei shadow_map_size = 1024;

  SunShadowMap sun;

  void update()
  {
    if (enabled)
    {
      if (!sun.shadow_texture
          || shadow_map_size != sun.shadow_texture->get_width())
        sun.setup(shadow_map_size);
    }
    else
    {
      if (sun.shadow_texture) sun.reset();
    }
  }

  void render(
    RenderContext& render_context, const PipelineContext& pipeline_context
  )
  {
    if (!enabled) return;

    // render sun shadow maps
    const std::size_t sun_shadows
      = std::min(render_context.sun_lights.size(), kSunShadowLimit);
    for (std::size_t i = 0; i < sun_shadows; ++i)
    {
      sun.render(
        *render_context.sun_lights[i].light, render_context, pipeline_context,
        shadow_map_size, shadow_distance, render_context.sun_lights[i]
      );
    }
  }
};

} // namespace axgl::impl::opengl::renderer
