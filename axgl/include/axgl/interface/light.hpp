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

  Light() : type(axgl::Light::Type::kSun) { }

  Light(const glm::vec3& direction, const axgl::Light::Color& color) :
    type(axgl::Light::Type::kSun), color(color), direction(direction)
  {
  }

  Light(const glm::vec3& position, const axgl::Light::Color& color, const axgl::Light::Strength& strength) :
    type(axgl::Light::Type::kPoint), color(color), strength(strength), position(position)
  {
  }

  Light(
    const glm::vec3& position,
    const glm::vec3& direction,
    const axgl::Light::Color& color,
    const axgl::Light::Strength& strength,
    float cut_off,
    float outer_cut_off) :
    type(axgl::Light::Type::kSpot),
    color(color),
    strength(strength),
    cut_off(glm::cos(glm::radians(cut_off))),
    outer_cut_off(glm::cos(glm::radians(outer_cut_off))),
    position(position),
    direction(direction)
  {
  }

  static axgl::Light sunlight(
    const glm::vec3& direction = glm::vec3(0.0f),
    const axgl::Light::Color& color = axgl::Light::Color{glm::vec3(0.3f), glm::vec3(1.0f), glm::vec3(1.0f)})
  {
    return {direction, color};
  }

  static axgl::Light point_light(
    const axgl::Light::Strength& strength = axgl::Light::Strength{1.0f, 0.09f, 0.032f},
    const axgl::Light::Color& color = axgl::Light::Color{glm::vec3(0.02f), glm::vec3(1.0f), glm::vec3(1.0f)},
    const glm::vec3& position = glm::vec3(0.0f))
  {
    return {position, color, strength};
  }

  static axgl::Light spotlight(
    const glm::vec3& direction = glm::vec3(0.0f),
    const float cut_off = 12.5f,
    const float outer_cut_off = 30.0f,
    const axgl::Light::Strength& strength = axgl::Light::Strength{1.0f, 0.09f, 0.032f},
    const axgl::Light::Color& color = axgl::Light::Color{glm::vec3(0.02f), glm::vec3(1.0f), glm::vec3(1.0f)},
    const glm::vec3& position = glm::vec3(0.0f))
  {
    return {position, direction, color, strength, cut_off, outer_cut_off};
  }
};

} // namespace axgl

