#pragma once

#include <memory>

#include "axgl/namespace.h"
#include "axgl/world/entity.h"
#include "axgl/world/render_context.h"
#include "axgl/opengl/model.h"
#include "axgl/opengl/texture.h"
#include "axgl/opengl/shader/skybox-shader.h"

NAMESPACE_OPENGL_ENTITY

class Skybox : public world::Entity
{
  std::shared_ptr<Model> model_;
  std::shared_ptr<shader::SkyboxShader> skybox_shader_;

public:
  Skybox(
    std::shared_ptr<shader::SkyboxShader> skybox_shader,
    std::shared_ptr<Texture> skybox_texture
  );
  void render(const world::RenderContext& context) override;
};

NAMESPACE_OPENGL_ENTITY_END
