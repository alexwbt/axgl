#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/light.hpp>

#include <glm/gtc/matrix_transform.hpp>

namespace axgl::impl::opengl
{

class CascadedShadowMap
{
public:
  static std::vector<glm::vec4> get_frustum_corners(const glm::mat4& pv)
  {
    const auto inverse_pv = glm::inverse(pv);
    std::vector<glm::vec4> frustum_corners;
    for (unsigned int x = 0; x < 2; ++x)
    {
      for (unsigned int y = 0; y < 2; ++y)
      {
        for (unsigned int z = 0; z < 2; ++z)
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

  static glm::mat4 get_sun_light_pv(const axgl::Light& light, const std::vector<glm::vec4>& frustum_corners)
  {
    const auto frustum_center = get_frustum_center(frustum_corners);
    const auto view = glm::lookAt(frustum_center + light.direction, frustum_center, glm::vec3(0.0f, 1.0f, 0.0f));

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();
    for (const auto& v : frustum_corners)
    {
      const auto transformed = view * v;
      minX = std::min(minX, transformed.x);
      maxX = std::max(maxX, transformed.x);
      minY = std::min(minY, transformed.y);
      maxY = std::max(maxY, transformed.y);
      minZ = std::min(minZ, transformed.z);
      maxZ = std::max(maxZ, transformed.z);
    }
    constexpr float zMult = 10.0f;
    if (minZ < 0) minZ *= zMult;
    else minZ /= zMult;
    if (maxZ < 0) maxZ /= zMult;
    else maxZ *= zMult;

    const auto projection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
    return projection * view;
  }
};

} // namespace axgl::impl::opengl
