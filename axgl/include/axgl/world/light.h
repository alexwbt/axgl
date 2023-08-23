#pragma once

#include <glm/glm.hpp>
#include "axgl/namespace.h"

NAMESPACE_WORLD

enum class LightType
{
  kSun, kPoint, kSpot
};

struct LightColor final
{
  glm::vec3 ambient, diffuse, specular;
};

struct LightStrength final
{
  float constant, linear, quadratic;
};

struct Light final
{
  LightType type;
  glm::vec3 position{ 0 };
  glm::vec3 direction{ 0 };
  glm::vec3 ambient, diffuse, specular;
  float constant = 0, linear = 0, quadratic = 0;
  float cutOff = 0, outerCutOff = 0;

  Light(glm::vec3 _direction, LightColor color)
    : type(LightType::kSun), direction(_direction),
    ambient(color.ambient), diffuse(color.diffuse), specular(color.specular)
  {}

  Light(glm::vec3 _position, LightColor color, LightStrength strength)
    : type(LightType::kPoint), position(_position),
    ambient(color.ambient), diffuse(color.diffuse), specular(color.specular),
    constant(strength.constant), linear(strength.linear), quadratic(strength.quadratic)
  {}

  Light(glm::vec3 _position, glm::vec3 _direction, LightColor color, LightStrength strength, float _cutOff, float _outerCutOff)
    : type(LightType::kSpot), position(_position), direction(_direction),
    ambient(color.ambient), diffuse(color.diffuse), specular(color.specular),
    constant(strength.constant), linear(strength.linear), quadratic(strength.quadratic),
    cutOff(glm::cos(glm::radians(cutOff))), outerCutOff(glm::cos(glm::radians(_outerCutOff)))
  {}
};

NAMESPACE_WORLD_END
