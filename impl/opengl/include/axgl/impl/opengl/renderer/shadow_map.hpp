#pragma once

#include <axgl/common.hpp>

#include <axgl/impl/opengl/renderer/csm.hpp>

#include <opengl/framebuffer.hpp>
#include <opengl/texture.hpp>

namespace axgl::impl::opengl::renderer
{

struct ShadowMap
{
  bool enable_shadow = false;
  bool enable_csm_debug = false;
  GLsizei shadow_map_cascade_levels = static_cast<GLsizei>(Csm::kCascadeCount);
  GLsizei shadow_map_size = 1024;
  // clamps the effective far used for cascade splitting so resolution
  // concentrates on the visible band rather than stretching to camera_far.
  float shadow_distance = 100.0f;
  std::unique_ptr<::opengl::Texture> shadow_texture;
  std::unique_ptr<::opengl::Framebuffer> shadow_framebuffer;

  // spot light shadows: one 2D depth texture per shadow-casting spot light.
  // allocated lazily up to spot_shadow_capacity; indexed by spot_shadow_index
  // on LightContext.
  GLsizei spot_shadow_capacity = 0;
  std::vector<std::unique_ptr<::opengl::Texture>> spot_shadow_textures;
  std::unique_ptr<::opengl::Framebuffer> spot_shadow_framebuffer;

  static constexpr GLsizei kMaxSpotShadows = 8;

  void setup()
  {
    // one 2D-array texture layer per cascade; the FBO attaches a single layer
    // at a time in the render loop below.
    shadow_texture = std::make_unique<::opengl::Texture>();
    shadow_texture->load_texture_array(
      0, GL_DEPTH_COMPONENT, shadow_map_size, shadow_map_size,
      shadow_map_cascade_levels, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr
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

  void ensure()
  {
    if (enable_shadow
        && (!shadow_texture || shadow_map_size != shadow_texture->get_width()))
      setup();
    else if (!enable_shadow && shadow_texture) reset();
  }

  void ensure_spot_shadows(GLsizei count)
  {
    if (!enable_shadow) return;
    if (count > kMaxSpotShadows) count = kMaxSpotShadows;
    if (count <= spot_shadow_capacity) return;
    spot_shadow_textures.clear();
    spot_shadow_textures.reserve(count);
    for (GLsizei i = 0; i < count; ++i)
    {
      auto tex = std::make_unique<::opengl::Texture>();
      tex->load_texture(
        0, GL_DEPTH_COMPONENT, shadow_map_size, shadow_map_size, 0,
        GL_DEPTH_COMPONENT, GL_FLOAT, nullptr
      );
      tex->set_parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      tex->set_parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      tex->set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
      tex->set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
      tex->set_parameter(
        GL_TEXTURE_BORDER_COLOR, std::array{1.0f, 1.0f, 1.0f, 1.0f}
      );
      tex->set_parameter(GL_TEXTURE_COMPARE_MODE, GL_NONE);
      spot_shadow_textures.push_back(std::move(tex));
    }
    spot_shadow_capacity = count;
    if (!spot_shadow_framebuffer)
      spot_shadow_framebuffer = std::make_unique<::opengl::Framebuffer>();
  }
};

} // namespace axgl::impl::opengl::renderer
