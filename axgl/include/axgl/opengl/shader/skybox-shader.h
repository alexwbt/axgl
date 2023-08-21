#pragma once

#include "axgl/namespace.h"
#include "axgl/opengl/shader_program.h"

NAMESPACE_OPENGL_SHADER

class SkyboxShader : public axgl::opengl::ShaderProgram
{
public:
  struct Uniforms : public ShaderProgram::Uniforms
  {
    glm::mat4 pv;
    int skybox_map;
  };

  struct Vertex final
  {
    glm::vec3 pos;
  };

public:
  SkyboxShader();

  void use(const ShaderProgram::Uniforms& uniforms) override;
  void enable_attributes() override;
};

NAMESPACE_OPENGL_SHADER_END
