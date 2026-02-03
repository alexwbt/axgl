#pragma once

#include <glm/glm.hpp>

namespace axgl
{

class Light final
{
public:
  enum class Type
  {
    kSun,
    kPoint,
    kSpot,
  };

  struct Color
  {
    glm::vec3 ambient = glm::vec3(1.0f);
    glm::vec3 diffuse = glm::vec3(1.0f);
    glm::vec3 specular = glm::vec3(1.0f);
  };

  struct Strength
  {
    float constant = 0.0f;
    float linear = 0.0f;
    float quadratic = 0.0f;
  };

  axgl::Light::Type type;
  axgl::Light::Color color;
  axgl::Light::Strength strength;
  float cut_off = 0.0f;
  float outer_cut_off = 0.0f;
  glm::vec3 position = glm::vec3(0.0f);
  glm::vec3 direction = glm::vec3(0.0f);
  bool casts_shadows = true;

  explicit Light(axgl::Light::Type type) : type(type) { }

  [[nodiscard]] glm::mat4 get_pv_matrix() const
  {
    const auto view = glm::lookAt(-direction, position, glm::vec3(0.0f, 1.0f, 0.0f));
    const auto projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f);
    return projection * view;
  }

  static axgl::Light sunlight(
    const glm::vec3& direction = glm::vec3(0.0f),
    const axgl::Light::Color& color = axgl::Light::Color{glm::vec3(0.3f), glm::vec3(1.0f), glm::vec3(1.0f)})
  {
    axgl::Light light(axgl::Light::Type::kSun);
    light.direction = direction;
    light.color = color;
    return light;
  }

  static axgl::Light point_light(
    const glm::vec3& position = glm::vec3(0.0f),
    const axgl::Light::Strength& strength = axgl::Light::Strength{1.0f, 0.09f, 0.032f},
    const axgl::Light::Color& color = axgl::Light::Color{glm::vec3(0.02f), glm::vec3(1.0f), glm::vec3(1.0f)})
  {
    axgl::Light light(axgl::Light::Type::kPoint);
    light.position = position;
    light.strength = strength;
    light.color = color;
    return light;
  }

  static axgl::Light spotlight(
    const glm::vec3& position = glm::vec3(0.0f),
    const glm::vec3& direction = glm::vec3(0.0f),
    const float cut_off = 12.5f,
    const float outer_cut_off = 30.0f,
    const axgl::Light::Strength& strength = axgl::Light::Strength{1.0f, 0.09f, 0.032f},
    const axgl::Light::Color& color = axgl::Light::Color{glm::vec3(0.02f), glm::vec3(1.0f), glm::vec3(1.0f)})
  {
    axgl::Light light(axgl::Light::Type::kSpot);
    light.position = position;
    light.direction = direction;
    light.color = color;
    light.strength = strength;
    light.cut_off = cut_off;
    light.outer_cut_off = outer_cut_off;
    return light;
  }
};

} // namespace axgl

