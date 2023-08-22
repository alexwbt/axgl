#include "axgl/opengl/shader/phong-shader.h"

NAMESPACE_OPENGL_SHADER

PhongShader::PhongShader() : ShaderProgram(
  { {GL_VERTEX_SHADER, "res/shaders/phong.vs"},
  {GL_FRAGMENT_SHADER, "res/shaders/phong.fs"} }
) {}

void PhongShader::use_light(std::shared_ptr<PhongShader::Light> light, int i)
{
  switch (light->type)
  {
  case PhongShader::SUN:
    set_vec3(("sunLights[" + std::to_string(i) + "].direction"), light->direction);
    set_vec3(("sunLights[" + std::to_string(i) + "].ambient"), light->ambient);
    set_vec3(("sunLights[" + std::to_string(i) + "].diffuse"), light->diffuse);
    set_vec3(("sunLights[" + std::to_string(i) + "].specular"), light->specular);
    break;
  case PhongShader::POINT:
    set_vec3(("lights[" + std::to_string(i) + "].position"), light->position);
    set_vec3(("lights[" + std::to_string(i) + "].ambient"), light->ambient);
    set_vec3(("lights[" + std::to_string(i) + "].diffuse"), light->diffuse);
    set_vec3(("lights[" + std::to_string(i) + "].specular"), light->specular);
    set_float(("lights[" + std::to_string(i) + "].constant"), light->constant);
    set_float(("lights[" + std::to_string(i) + "].linear"), light->linear);
    set_float(("lights[" + std::to_string(i) + "].quadratic"), light->quadratic);
    break;
  case PhongShader::SPOT:
    set_vec3(("spotLights[" + std::to_string(i) + "].direction"), light->direction);
    set_vec3(("spotLights[" + std::to_string(i) + "].position"), light->position);
    set_vec3(("spotLights[" + std::to_string(i) + "].ambient"), light->ambient);
    set_vec3(("spotLights[" + std::to_string(i) + "].diffuse"), light->diffuse);
    set_vec3(("spotLights[" + std::to_string(i) + "].specular"), light->specular);
    set_float(("spotLights[" + std::to_string(i) + "].constant"), light->constant);
    set_float(("spotLights[" + std::to_string(i) + "].linear"), light->linear);
    set_float(("spotLights[" + std::to_string(i) + "].quadratic"), light->quadratic);
    set_float(("spotLights[" + std::to_string(i) + "].cutOff"), light->cutOff);
    set_float(("spotLights[" + std::to_string(i) + "].outerCutOff"), light->outerCutOff);
    break;
  }
}

void PhongShader::use_uniforms(const Uniforms& data)
{
  int sizes[3] = { 0, 0, 0 };
  for (auto& light : data.lights)
  {
    int type = (int)(light->type);
    use_light(std::move(light), sizes[type]);
    sizes[type]++;
  }
  set_int("sunLights_size", sizes[0]);
  set_int("lights_size", sizes[1]);
  set_int("spotLights_size", sizes[2]);

  set_vec3("cameraPos", data.camera_pos);

  set_int("diffuseMap", data.diffuse_map);
  set_float("specular", data.specular);
  set_float("shininess", data.shininess);

  set_mat4("mvp", data.mvp);
  set_mat4("model", data.model);
}

NAMESPACE_OPENGL_SHADER_END
