#pragma once

#include <array>
#include <memory>

#include <axgl/common.hpp>

#include <opengl/framebuffer.hpp>
#include <opengl/texture.hpp>

namespace axgl::impl::opengl::renderer
{

inline constexpr std::size_t kBloomMipLevels = 5;

struct Bloom
{
  bool enabled = false;

  float threshold = 1.0f;
  float intensity = 0.3f;

  // bright-pass extraction output (full resolution)
  std::unique_ptr<::opengl::Texture> bright_texture;
  std::unique_ptr<::opengl::Framebuffer> bright_framebuffer;

  // composite output: the bloom result is composited here (not back into
  // screen_texture directly, which would be feedback-undefined). The renderer
  // blits this back into screen_texture after compositing.
  std::unique_ptr<::opengl::Texture> composite_texture;
  std::unique_ptr<::opengl::Framebuffer> composite_framebuffer;

  // mip chain: downsampled ping-pong textures + framebuffers for iterative
  // Gaussian blur. index [i][0] and [i][1] are the two ping-pong targets.
  std::array<std::array<std::unique_ptr<::opengl::Texture>, 2>, kBloomMipLevels>
    mip_textures;
  std::array<
    std::array<std::unique_ptr<::opengl::Framebuffer>, 2>,
    kBloomMipLevels>
    mip_framebuffers;

  glm::ivec2 last_viewport{0};

  void setup(const glm::ivec2& viewport)
  {
    last_viewport = viewport;

    // bright-pass texture (full resolution, HDR)
    bright_texture = std::make_unique<::opengl::Texture>();
    bright_texture->load_texture(
      0, GL_RGBA16F, viewport.x, viewport.y, 0, GL_RGBA, GL_HALF_FLOAT, nullptr
    );
    bright_texture->set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    bright_texture->set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    bright_texture->set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    bright_texture->set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    bright_framebuffer = std::make_unique<::opengl::Framebuffer>();
    bright_framebuffer->attach_texture(GL_COLOR_ATTACHMENT0, *bright_texture);

    // composite texture (full resolution, HDR) — result of screen + bloom
    composite_texture = std::make_unique<::opengl::Texture>();
    composite_texture->load_texture(
      0, GL_RGBA16F, viewport.x, viewport.y, 0, GL_RGBA, GL_HALF_FLOAT, nullptr
    );
    composite_texture->set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    composite_texture->set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    composite_texture->set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    composite_texture->set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    composite_framebuffer = std::make_unique<::opengl::Framebuffer>();
    composite_framebuffer->attach_texture(
      GL_COLOR_ATTACHMENT0, *composite_texture
    );

    // mip chain: each level is half the previous
    glm::ivec2 mip_size = viewport;
    for (std::size_t i = 0; i < kBloomMipLevels; ++i)
    {
      mip_size = glm::max(mip_size / 2, glm::ivec2(1));
      for (std::size_t j = 0; j < 2; ++j)
      {
        mip_textures[i][j] = std::make_unique<::opengl::Texture>();
        mip_textures[i][j]->load_texture(
          0, GL_RGBA16F, mip_size.x, mip_size.y, 0, GL_RGBA, GL_HALF_FLOAT,
          nullptr
        );
        mip_textures[i][j]->set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        mip_textures[i][j]->set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        mip_textures[i][j]->set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        mip_textures[i][j]->set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        mip_framebuffers[i][j] = std::make_unique<::opengl::Framebuffer>();
        mip_framebuffers[i][j]->attach_texture(
          GL_COLOR_ATTACHMENT0, *mip_textures[i][j]
        );
      }
    }
  }

  void reset()
  {
    for (std::size_t i = 0; i < kBloomMipLevels; ++i)
    {
      mip_framebuffers[i][0].reset();
      mip_framebuffers[i][1].reset();
      mip_textures[i][0].reset();
      mip_textures[i][1].reset();
    }
    bright_framebuffer.reset();
    bright_texture.reset();
    composite_framebuffer.reset();
    composite_texture.reset();
  }

  void update(const glm::ivec2& viewport)
  {
    if (enabled && !bright_framebuffer) setup(viewport);
    else if (enabled && bright_framebuffer && viewport != last_viewport)
    {
      reset();
      setup(viewport);
    }
    else if (!enabled && bright_framebuffer) reset();
  }

  [[nodiscard]] glm::ivec2 mip_size(const std::size_t level) const
  {
    glm::ivec2 size = last_viewport;
    for (std::size_t i = 0; i <= level; ++i)
      size = glm::max(size / 2, glm::ivec2(1));
    return size;
  }
};

} // namespace axgl::impl::opengl::renderer
