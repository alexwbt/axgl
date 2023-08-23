#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "axgl/namespace.h"
#include "axgl/world/light.h"
#include "axgl/opengl/shader_program.h"

NAMESPACE_OPENGL_SHADER

class PhongShader : public ShaderProgram
{
public:
  struct Uniforms
  {
    std::vector<std::shared_ptr<axgl::world::Light>> lights;
    glm::vec3 camera_pos;
    float specular;
    float shininess;
    glm::mat4 mvp;
    glm::mat4 model;
  };

public:
  PhongShader(const std::string& name);
  void use_uniforms(const Uniforms& uniforms);
  void use_light(std::shared_ptr<axgl::world::Light> light, int i);

};

NAMESPACE_OPENGL_SHADER_END
