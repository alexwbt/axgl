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
  std::shared_ptr<shader::SkyboxShader> skybox_shader,
  std::shared_ptr<Texture> skybox_texture
) : skybox_shader_(std::move(skybox_shader))
{
  std::vector<Vertex2> vertices;
  vertices.reserve(kSkyboxVerticesSize / 3);
  for (int i = 0; i < kSkyboxVerticesSize; i += 3)
    vertices.push_back({ {
        kSkyboxVertices[i],
        kSkyboxVertices[i + 1],
        kSkyboxVertices[i + 2]} });

  auto mesh = std::make_shared<Mesh>(vertices);
  mesh->add_texture(skybox_texture);

  model_ = std::make_shared<Model>();
  model_->add_mesh(mesh);
}

void Skybox::render(const world::RenderContext& context)
{
  glm::mat4 view = glm::mat4(glm::mat3(context.view));

  shader::SkyboxShader::Uniforms uniforms;
  uniforms.pv = context.projection * view;
  uniforms.skybox_map = 0;

  glDepthMask(GL_FALSE);

  skybox_shader_->use_program();
  skybox_shader_->use_uniforms(uniforms);
  model_->render(skybox_shader_);

  glDepthMask(GL_TRUE);
}

NAMESPACE_OPENGL_ENTITY_END
