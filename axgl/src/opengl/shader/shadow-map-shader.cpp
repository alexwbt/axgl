#include "axgl/opengl/shader/shadow-map-shader.h"

NAMESPACE_OPENGL_SHADER

ShadowMapShader::ShadowMapShader() : ShaderProgram(
  { {GL_VERTEX_SHADER, "res/shaders/mvp.vs"} }
) {}

void ShadowMapShader::use_uniforms(const Uniforms& data)
{
  set_mat4("mvp", data.mvp);
}

NAMESPACE_OPENGL_SHADER_END
