#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "axgl/namespace.h"
#include "axgl/opengl/shader_program.h"

NAMESPACE_OPENGL_SHADER

class PhongShader : public ShaderProgram
{
public:
  enum LightType
  {
    SUN, POINT, SPOT
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
      : type(LightType::SUN), direction(_direction),
      ambient(color.ambient), diffuse(color.diffuse), specular(color.specular)
    {}

    Light(glm::vec3 _position, LightColor color, LightStrength strength)
      : type(LightType::POINT), position(_position),
      ambient(color.ambient), diffuse(color.diffuse), specular(color.specular),
      constant(strength.constant), linear(strength.linear), quadratic(strength.quadratic)
    {}

    Light(glm::vec3 _position, glm::vec3 _direction, LightColor color, LightStrength strength, float _cutOff, float _outerCutOff)
      : type(LightType::SPOT), position(_position), direction(_direction),
      ambient(color.ambient), diffuse(color.diffuse), specular(color.specular),
      constant(strength.constant), linear(strength.linear), quadratic(strength.quadratic),
      cutOff(glm::cos(glm::radians(cutOff))), outerCutOff(glm::cos(glm::radians(_outerCutOff)))
    {}
  };

  struct Uniforms : public ShaderProgram::Uniforms
  {
    std::vector<std::shared_ptr<Light>> lights;
    glm::vec3 camera_pos;
    int diffuse_map;
    float specular;
    float shininess;
    glm::mat4 mvp;
    glm::mat4 model;
  };

  struct Vertex final
  {
    glm::vec3 pos;
    glm::vec3 normal;
    // glm::vec3 color;
    glm::vec2 uv;
  };

public:
  PhongShader();

  void enable_attributes() override;

private:
  void use_light(std::shared_ptr<Light> light, int i);
  void use(const ShaderProgram::Uniforms& uniforms) override;

};

NAMESPACE_OPENGL_SHADER_END
