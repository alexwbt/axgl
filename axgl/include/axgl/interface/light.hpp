#pragma once

#include <glm/glm.hpp>

namespace axgl
{

class Light final
{
public:
  enum Type
  {
    kSun,
    kPoint,
    kSpot
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

  Type type;
  Color color;
  Strength strength;
  float cut_off = 0.0f;
  float outer_cut_off = 0.0f;
  glm::vec3 position = glm::vec3(0.0f);
  glm::vec3 direction = glm::vec3(0.0f);

  Light() : type(kSun) { }

  Light(glm::vec3 direction, Color color) : type(kSun), color(color), direction(direction) { }

  Light(glm::vec3 position, Color color, Strength strength) :
    type(kPoint), color(color), strength(strength), position(position)
  {
  }

  Light(glm::vec3 position, glm::vec3 direction, Color color, Strength strength, float cut_off, float outer_cut_off) :
    type(kSpot),
    color(color),
    strength(strength),
    cut_off(glm::cos(glm::radians(cut_off))),
    outer_cut_off(glm::cos(glm::radians(outer_cut_off))),
    position(position),
    direction(direction)
  {
  }
};

} // namespace axgl

