#pragma once

#include <array>
#include <limits>

#include <axgl/common.hpp>
#include <axgl/interface/light.hpp>

#include <glm/gtc/matrix_transform.hpp>

namespace axgl::impl::opengl
{

class CascadedShadowMap
{
public:
  static constexpr std::size_t kCascadeCount = 3;

  struct Cascade
  {
    glm::mat4 light_pv{0.0f};
    float split_near = 0.0f;
    float split_far = 0.0f;
  };

  static std::array<Cascade, kCascadeCount> compute_cascades(
    const axgl::Light& light,
    const glm::mat4& camera_inverse_pv,
    const float camera_near,
    const float camera_far,
    const float shadow_far,
    const float lambda = 0.5f)
  {
    std::array<Cascade, kCascadeCount> cascades;

    const float effective_far = std::min(camera_far, shadow_far);

    // practical split: blend logarithmic (more near precision) and uniform
    // (even spread) partitions via lambda. splits use effective_far so
    // cascades concentrate on the visible band instead of stretching to
    // camera_far (e.g. 1000).
    std::array<float, kCascadeCount + 1> split_distances{};
    split_distances[0] = camera_near;
    split_distances[kCascadeCount] = effective_far;
    for (std::size_t i = 1; i < kCascadeCount; ++i)
    {
      const float p = static_cast<float>(i) / static_cast<float>(kCascadeCount);
      const float log_split = camera_near * std::pow(effective_far / camera_near, p);
      const float linear_split = camera_near + (effective_far - camera_near) * p;
      split_distances[i] = lambda * log_split + (1.0f - lambda) * linear_split;
    }

    for (std::size_t c = 0; c < kCascadeCount; ++c)
    {
      const float near_dist = split_distances[c];
      const float far_dist = split_distances[c + 1];

      cascades[c].split_near = near_dist;
      cascades[c].split_far = far_dist;

      // unproject the full camera frustum's 8 NDC corners into world space via
      // the inverse camera PV. these span [camera_near, camera_far], NOT
      // effective_far, so the interpolation ratios below must use camera_far.
      const std::array<glm::vec4, 8> ndc_corners{{
        {-1.0f, -1.0f, -1.0f, 1.0f},
        {1.0f, -1.0f, -1.0f, 1.0f},
        {1.0f, 1.0f, -1.0f, 1.0f},
        {-1.0f, 1.0f, -1.0f, 1.0f},
        {-1.0f, -1.0f, 1.0f, 1.0f},
        {1.0f, -1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f},
        {-1.0f, 1.0f, 1.0f, 1.0f},
      }};

      std::array<glm::vec3, 8> frustum_corners{};
      for (std::size_t i = 0; i < 8; ++i)
      {
        glm::vec4 world = camera_inverse_pv * ndc_corners[i];
        world /= world.w;
        frustum_corners[i] = glm::vec3(world);
      }

      // interpolate the 4 near-plane and 4 far-plane corners to this slice's
      // [near_dist, far_dist]. ratios use camera_far (the PV's real far) since
      // the corners above unproject to camera_far, not effective_far. using
      // effective_far here was the resolution-killer bug: a 0.05 ratio landed
      // corners at 5% of 1000 = 50 units instead of ~0.5 units.
      const float near_ratio = (near_dist - camera_near) / (camera_far - camera_near);
      const float far_ratio = (far_dist - camera_near) / (camera_far - camera_near);
      for (std::size_t i = 0; i < 4; ++i)
      {
        const glm::vec3 near_c = frustum_corners[i];
        const glm::vec3 far_c = frustum_corners[i + 4];
        frustum_corners[i] = glm::mix(near_c, far_c, near_ratio);
        frustum_corners[i + 4] = glm::mix(near_c, far_c, far_ratio);
      }

      std::array<glm::vec4, 8> corners{};
      for (std::size_t i = 0; i < 8; ++i)
        corners[i] = glm::vec4(frustum_corners[i], 1.0f);

      glm::vec3 center(0.0f);
      for (const auto& v : corners)
        center += glm::vec3(v);
      center /= static_cast<float>(corners.size());

    // light.direction points from the light toward the scene, so the eye is at
    // center - light_dir (looking back toward center). center + light_dir
    // would place the light on the wrong side looking up through the floor.
    const glm::vec3 light_dir = glm::normalize(light.direction);
    const glm::mat4 light_view = glm::lookAt(center - light_dir, center, glm::vec3(0.0f, 1.0f, 0.0f));

      float min_x = std::numeric_limits<float>::max();
      float max_x = std::numeric_limits<float>::lowest();
      float min_y = std::numeric_limits<float>::max();
      float max_y = std::numeric_limits<float>::lowest();
      float min_z = std::numeric_limits<float>::max();
      float max_z = std::numeric_limits<float>::lowest();
      for (const auto& v : corners)
      {
        const glm::vec3 t = glm::vec3(light_view * v);
        min_x = std::min(min_x, t.x);
        max_x = std::max(max_x, t.x);
        min_y = std::min(min_y, t.y);
        max_y = std::max(max_y, t.y);
        min_z = std::min(min_z, t.z);
        max_z = std::max(max_z, t.z);
      }

      // pad z so shadow casters just outside the slice still project into the
      // depth range. too large wrecks depth precision (acne); too small clips
      // off-slice casters.
      constexpr float z_padding = 1.0f;
      min_z -= z_padding;
      max_z += z_padding;

      const glm::mat4 light_proj = glm::ortho(min_x, max_x, min_y, max_y, min_z, max_z);
      cascades[c].light_pv = light_proj * light_view;
    }

    return cascades;
  }
};

} // namespace axgl::impl::opengl
