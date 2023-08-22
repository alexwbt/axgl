#pragma once

#include "axgl/namespace.h"
#include "axgl/opengl/shader_program.h"

NAMESPACE_OPENGL_SHADER

class SkyboxShader : public axgl::opengl::ShaderProgram
{
public:
  struct Uniforms
  {
    glm::mat4 pv;
    int skybox_map;
  };

public:
  SkyboxShader();
  void use_uniforms(const Uniforms& uniforms);
};

NAMESPACE_OPENGL_SHADER_END
