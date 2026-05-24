#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/light.hpp>

#include <glm/gtc/matrix_transform.hpp>

namespace axgl::impl::opengl
{

class CascadedShadowMap
{
  static std::vector<glm::vec4> get_frustum_corners(const glm::mat4& camera_pv)
  {
    const auto inverse_pv = glm::inverse(camera_pv);
    std::vector<glm::vec4> frustum_corners;
    for (int x = 0; x < 2; ++x)
    {
      for (int y = 0; y < 2; ++y)
      {
        for (int z = 0; z < 2; ++z)
        {
          const glm::vec4 point = inverse_pv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
          frustum_corners.push_back(point / point.w);
        }
      }
    }
    return frustum_corners;
  }

  static glm::vec3 get_frustum_center(const std::vector<glm::vec4>& frustum_corners)
  {
    glm::vec3 center(0.0f);
    for (const auto& v : frustum_corners)
      center += glm::vec3(v);
    center /= frustum_corners.size();
    return center;
  }

public:
  static glm::mat4 get_sun_light_pv(const axgl::Light& light, const glm::mat4& camera_pv)
  {
    const auto frustum_corners = get_frustum_corners(camera_pv);
    const auto frustum_center = get_frustum_center(frustum_corners);
    const auto view = glm::lookAt(frustum_center + light.direction, frustum_center, glm::vec3(0.0f, 1.0f, 0.0f));

    float min_x = std::numeric_limits<float>::max();
    float max_x = std::numeric_limits<float>::lowest();
    float min_y = std::numeric_limits<float>::max();
    float max_y = std::numeric_limits<float>::lowest();
    float min_z = std::numeric_limits<float>::max();
    float max_z = std::numeric_limits<float>::lowest();
    for (const auto& v : frustum_corners)
    {
      const auto transformed = view * v;
      min_x = std::min(min_x, transformed.x);
      max_x = std::max(max_x, transformed.x);
      min_y = std::min(min_y, transformed.y);
      max_y = std::max(max_y, transformed.y);
      min_z = std::min(min_z, transformed.z);
      max_z = std::max(max_z, transformed.z);
    }
    constexpr float z_factor = 10.0f;
    if (min_z < 0) min_z *= z_factor;
    else min_z /= z_factor;
    if (max_z < 0) max_z /= z_factor;
    else max_z *= z_factor;

    const auto projection = glm::ortho(min_x, max_x, min_y, max_y, min_z, max_z);
    return projection * view;
  }
};

} // namespace axgl::impl::opengl
