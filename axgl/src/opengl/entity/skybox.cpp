#include "axgl/opengl/entity/skybox.h"

#include <vector>

#include "axgl/opengl/static_resource.h"
#include "axgl/opengl/shader/skybox-shader.h"

NAMESPACE_OPENGL_ENTITY

static constexpr int kSkyboxVerticesSize = 108;
static constexpr float kSkyboxVertices[kSkyboxVerticesSize] = {
  -1.0f,  1.0f, -1.0f,
  -1.0f, -1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,
   1.0f,  1.0f, -1.0f,
  -1.0f,  1.0f, -1.0f,

  -1.0f, -1.0f,  1.0f,
  -1.0f, -1.0f, -1.0f,
  -1.0f,  1.0f, -1.0f,
  -1.0f,  1.0f, -1.0f,
  -1.0f,  1.0f,  1.0f,
  -1.0f, -1.0f,  1.0f,

   1.0f, -1.0f, -1.0f,
   1.0f, -1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f,  1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,

  -1.0f, -1.0f,  1.0f,
  -1.0f,  1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f, -1.0f,  1.0f,
  -1.0f, -1.0f,  1.0f,

  -1.0f,  1.0f, -1.0f,
   1.0f,  1.0f, -1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
  -1.0f,  1.0f,  1.0f,
  -1.0f,  1.0f, -1.0f,

  -1.0f, -1.0f, -1.0f,
  -1.0f, -1.0f,  1.0f,
   1.0f, -1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,
  -1.0f, -1.0f,  1.0f,
   1.0f, -1.0f,  1.0f
};

Skybox::Skybox(
  std::shared_ptr<ShaderProgram> skybox_shader,
  std::shared_ptr<Texture> skybox_texture)
{
  std::vector<shader::SkyboxShader::Vertex> vertices;
  vertices.reserve(kSkyboxVerticesSize / 3);
  for (int i = 0; i < kSkyboxVerticesSize; i += 3)
    vertices.push_back({ {
        kSkyboxVertices[i],
        kSkyboxVertices[i + 1],
        kSkyboxVertices[i + 2]} });

  auto textures = std::make_shared<std::vector<std::shared_ptr<Texture>>>(1);
  textures->at(0) = skybox_texture;

  model_ = std::make_shared<Model>(vertices, std::move(skybox_shader), textures);
}

void Skybox::render(const RenderContext& context)
{
  glm::mat4 view = glm::mat4(glm::mat3(context.view));

  shader::SkyboxShader::Uniforms uniforms;
  uniforms.pv = context.projection * view;
  uniforms.skybox_map = 0;

  glDepthMask(GL_FALSE);
  glCullFace(GL_BACK);

  model_->render(uniforms);

  glCullFace(GL_FRONT);
  glDepthMask(GL_TRUE);
}

NAMESPACE_OPENGL_ENTITY_END
