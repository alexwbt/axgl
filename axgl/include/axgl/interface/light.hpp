#pragma once

#include "axgl/common.hpp"

#include <glm/glm.hpp>

NAMESPACE_AXGL_INTERFACE

class Light final
{
public:
  enum Type
  {
    SUN, POINT, SPOT
  };

  struct Color
  {
    glm::vec3 ambient = glm::vec3(0.0f);
    glm::vec3 diffuse = glm::vec3(0.0f);
    glm::vec3 specular = glm::vec3(0.0f);
  };

  struct Strength
  {
    float constant = 0.0f;
    float linear = 0.0f;
    float quadratic = 0.0f;
  };

public:
  Type type;
  Color color;
  Strength strength;
  float cut_off = 0.0f;
  float outer_cut_off = 0.0f;
  glm::vec3 position = glm::vec3(0.0f);
  glm::vec3 direction = glm::vec3(0.0f);

  Light(glm::vec3 direction, Color color) :
    type(Type::SUN), direction(direction), color(color)
  {}

  Light(glm::vec3 position, Color color, Strength strength) :
    type(Type::POINT), position(position), color(color), strength(strength)
  {}

  Light(glm::vec3 position, glm::vec3 direction, Color color, Strength strength, float cut_off, float outer_cut_off) :
    type(Type::SPOT), position(position), direction(direction), color(color), strength(strength),
    cut_off(glm::cos(glm::radians(cut_off))), outer_cut_off(glm::cos(glm::radians(outer_cut_off)))
  {}
};

NAMESPACE_AXGL_INTERFACE_END
