#pragma once

#include <axgl/impl/opengl/renderer/point_shadow_map.hpp>
#include <axgl/impl/opengl/renderer/spot_shadow_map.hpp>
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
  SpotShadowMap spot;
  PointShadowMap point;

  void update()
  {
    if (enabled)
    {
      if (!sun.shadow_texture
          || shadow_map_size != sun.shadow_texture->get_width())
        sun.setup(shadow_map_size);
      if (!spot.shadow_texture
          || shadow_map_size != spot.shadow_texture->get_width())
        spot.setup(shadow_map_size);
      if (!point.shadow_texture
          || shadow_map_size != point.shadow_texture->get_width())
        point.setup(shadow_map_size);
    }
    else
    {
      if (sun.shadow_texture) sun.reset();
      if (spot.shadow_texture) spot.reset();
      if (point.shadow_texture) point.reset();
    }
  }

  // Render shadow maps and populate render context
  void render_shadow_pass(
    RenderContext& render_context, const PipelineContext& pipeline_context
  )
  {
    if (!enabled) return;

    // sun light shadow
    std::size_t sun_shadow_count = 0;
    for (auto& e : render_context.sun_lights)
    {
      if (e.light->casts_shadows)
      {
        sun.render(
          e, render_context, pipeline_context, shadow_map_size, shadow_distance
        );
        if (++sun_shadow_count >= kSunShadowLimit) break;
      }
    }
    render_context.sun_shadow_maps = sun.shadow_texture.get();

    // spot light shadow
    std::size_t spot_shadow_count = 0;
    for (auto& e : render_context.spot_lights)
    {
      if (e.light->casts_shadows && spot_shadow_count < kSpotShadowLimit)
      {
        e.shadow_index = static_cast<std::int32_t>(spot_shadow_count);
        spot.render(e, shadow_map_size, shadow_distance, pipeline_context);
        ++spot_shadow_count;
      }
      else e.shadow_index = -1;
    }
    render_context.spot_shadow_maps = spot.shadow_texture.get();

    // point light shadow
    std::size_t point_shadow_count = 0;
    for (auto& e : render_context.point_lights)
    {
      if (e.light->casts_shadows && point_shadow_count < kPointShadowLimit)
      {
        e.shadow_index = static_cast<std::int32_t>(point_shadow_count);
        point.render(e, shadow_map_size, shadow_distance, pipeline_context);
        ++point_shadow_count;
      }
      else e.shadow_index = -1;
    }
    render_context.point_shadow_maps = point.shadow_texture.get();
  }
};

} // namespace axgl::impl::opengl::renderer
