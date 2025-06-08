#pragma once

#include <glm/glm.hpp>

#include <axgl/common.hpp>

NAMESPACE_AXGL_INTERFACE

class Light final
{
public:
  enum Type
  {
    kSun, kPoint, kSpot
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

public:
  Type type;
  Color color;
  Strength strength;
  float cut_off = 0.0f;
  float outer_cut_off = 0.0f;
  glm::vec3 position = glm::vec3(0.0f);
  glm::vec3 direction = glm::vec3(0.0f);

  Light(): type(Type::kSun) {}

  Light(glm::vec3 direction, Color color) :
    type(Type::kSun), direction(direction), color(color)
  {}

  Light(glm::vec3 position, Color color, Strength strength) :
    type(Type::kPoint), position(position), color(color), strength(strength)
  {}

  Light(glm::vec3 position, glm::vec3 direction, Color color, Strength strength, float cut_off, float outer_cut_off) :
    type(Type::kSpot), position(position), direction(direction), color(color), strength(strength),
    cut_off(glm::cos(glm::radians(cut_off))), outer_cut_off(glm::cos(glm::radians(outer_cut_off)))
  {}
};

NAMESPACE_AXGL_INTERFACE_END
