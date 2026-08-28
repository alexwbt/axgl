#pragma once

#include <array>
#include <memory>
#include <random>

#include <axgl/common.hpp>

#include <opengl/framebuffer.hpp>
#include <opengl/texture.hpp>

namespace axgl::impl::opengl::renderer
{

inline constexpr std::size_t kSsaoKernelSize = 64;
inline constexpr std::size_t kSsaoNoiseSize = 16;

struct SSAO
{
  bool enabled = false;

  float radius = 0.5f;
  float bias = 0.025f;

  std::array<glm::vec3, kSsaoKernelSize> kernel{};
  std::array<glm::vec3, kSsaoNoiseSize * kSsaoNoiseSize> noise{};

  std::unique_ptr<::opengl::Texture> position_texture;
  std::unique_ptr<::opengl::Texture> normal_texture;
  std::unique_ptr<::opengl::Framebuffer> geometry_framebuffer;

  std::unique_ptr<::opengl::Texture> noise_texture;

  std::unique_ptr<::opengl::Texture> ssao_texture;
  std::unique_ptr<::opengl::Framebuffer> ssao_framebuffer;

  std::unique_ptr<::opengl::Texture> blur_texture;
  std::unique_ptr<::opengl::Framebuffer> blur_framebuffer;

  void generate_kernel()
  {
    auto lerp = [](float a, float b, float f) { return a + f * (b - a); };

    std::uniform_real_distribution<float> dist01(0.0f, 1.0f);
    std::uniform_real_distribution<float> dist11(-1.0f, 1.0f);
    std::mt19937 rng(std::random_device{}());

    for (std::size_t i = 0; i < kSsaoKernelSize; ++i)
    {
      glm::vec3 sample(dist11(rng), dist11(rng), dist01(rng));
      sample = glm::normalize(sample) * dist01(rng);
      float scale = static_cast<float>(i) / kSsaoKernelSize;
      scale = lerp(0.1f, 1.0f, scale * scale);
      kernel[i] = sample * scale;
    }
  }

  void generate_noise()
  {
    std::uniform_real_distribution<float> dist11(-1.0f, 1.0f);
    std::mt19937 rng(std::random_device{}());
    for (auto& n : noise)
      n = glm::vec3(dist11(rng), dist11(rng), 0.0f);
  }

  void setup(const glm::ivec2& viewport, ::opengl::Texture& depth_texture)
  {
    generate_kernel();
    generate_noise();

    // geometry pass: view-space position + normal (MRT). Shares the screen
    // depth texture so the pass gets correct depth testing against the scene.
    position_texture = std::make_unique<::opengl::Texture>();
    position_texture->load_texture(
      0, GL_RGB16F, viewport.x, viewport.y, 0, GL_RGB, GL_FLOAT, nullptr
    );
    position_texture->set_parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    position_texture->set_parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    normal_texture = std::make_unique<::opengl::Texture>();
    normal_texture->load_texture(
      0, GL_RGB16F, viewport.x, viewport.y, 0, GL_RGB, GL_FLOAT, nullptr
    );
    normal_texture->set_parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    normal_texture->set_parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    geometry_framebuffer = std::make_unique<::opengl::Framebuffer>();
    geometry_framebuffer->attach_texture(
      GL_COLOR_ATTACHMENT0, *position_texture
    );
    geometry_framebuffer->attach_texture(GL_COLOR_ATTACHMENT1, *normal_texture);
    geometry_framebuffer->attach_texture(GL_DEPTH_ATTACHMENT, depth_texture);
    geometry_framebuffer->set_draw_buffers(
      {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1}
    );

    // SSAO noise tile: a small texture wrapped across the screen via
    // noise_scale = viewport / noise_size
    noise_texture = std::make_unique<::opengl::Texture>();
    noise_texture->load_texture(
      0, GL_RGB16F, static_cast<GLsizei>(kSsaoNoiseSize),
      static_cast<GLsizei>(kSsaoNoiseSize), 0, GL_RGB, GL_FLOAT, noise.data()
    );
    noise_texture->set_parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    noise_texture->set_parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    noise_texture->set_parameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
    noise_texture->set_parameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

    // SSAO color output (R8 enough: single occlusion factor)
    ssao_texture = std::make_unique<::opengl::Texture>();
    ssao_texture->load_texture(
      0, GL_RED, viewport.x, viewport.y, 0, GL_RED, GL_FLOAT, nullptr
    );
    ssao_texture->set_parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    ssao_texture->set_parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    ssao_framebuffer = std::make_unique<::opengl::Framebuffer>();
    ssao_framebuffer->attach_texture(GL_COLOR_ATTACHMENT0, *ssao_texture);

    // blurred SSAO output
    blur_texture = std::make_unique<::opengl::Texture>();
    blur_texture->load_texture(
      0, GL_RED, viewport.x, viewport.y, 0, GL_RED, GL_FLOAT, nullptr
    );
    blur_texture->set_parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    blur_texture->set_parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    blur_framebuffer = std::make_unique<::opengl::Framebuffer>();
    blur_framebuffer->attach_texture(GL_COLOR_ATTACHMENT0, *blur_texture);
  }

  void reset()
  {
    blur_framebuffer.reset();
    blur_texture.reset();
    ssao_framebuffer.reset();
    ssao_texture.reset();
    noise_texture.reset();
    geometry_framebuffer.reset();
    normal_texture.reset();
    position_texture.reset();
  }

  void update(const glm::ivec2& viewport, ::opengl::Texture& depth_texture)
  {
    if (enabled && !ssao_framebuffer) setup(viewport, depth_texture);
    else if (!enabled && ssao_framebuffer) reset();
  }

  [[nodiscard]] glm::vec2 noise_scale(const glm::ivec2& viewport) const
  {
    return glm::vec2(viewport) / static_cast<float>(kSsaoNoiseSize);
  }
};

} // namespace axgl::impl::opengl::renderer
