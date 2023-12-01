#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "axgl/namespace.h"
#include "axgl/opengl/shader_program.h"

NAMESPACE_OPENGL_SHADER

class ShadowMapShader : public ShaderProgram
{
public:
  struct Uniforms
  {
    glm::mat4 mvp;
  };

public:
  ShadowMapShader();
  void use_uniforms(const Uniforms& uniforms);

};

NAMESPACE_OPENGL_SHADER_END
