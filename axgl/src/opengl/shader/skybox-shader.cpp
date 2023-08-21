#include "axgl/opengl/shader/skybox-shader.h"
#include <exception>
#include <spdlog/spdlog.h>

NAMESPACE_OPENGL_SHADER

SkyboxShader::SkyboxShader() : ShaderProgram(
  { {GL_VERTEX_SHADER, "res/shaders/skybox.vs"},
  {GL_FRAGMENT_SHADER, "res/shaders/skybox.fs"} }
) {}

void SkyboxShader::use(const ShaderProgram::Uniforms& uniforms)
{
  use_program();

  try
  {
    auto data = dynamic_cast<const Uniforms&>(uniforms);

    set_int("skybox", data.skybox_map);
    set_mat4("pv", data.pv);
  }
  catch (const std::bad_cast& exception)
  {
    SPDLOG_ERROR("Failed to cast skybox shader uniform: {}", exception.what());
  }
}

void SkyboxShader::enable_attributes()
{
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(struct Vertex, pos));
}

NAMESPACE_OPENGL_SHADER_END
