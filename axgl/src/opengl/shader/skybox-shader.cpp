#include "axgl/opengl/shader/skybox-shader.h"

NAMESPACE_OPENGL_SHADER

SkyboxShader::SkyboxShader() : ShaderProgram(
  { {GL_VERTEX_SHADER, "res/shaders/skybox.vs"},
  {GL_FRAGMENT_SHADER, "res/shaders/skybox.fs"} }
) {}

void SkyboxShader::use_uniforms(const Uniforms& data)
{
  set_int("skybox", data.skybox_map);
  set_mat4("pv", data.pv);
}

NAMESPACE_OPENGL_SHADER_END
