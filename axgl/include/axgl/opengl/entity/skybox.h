#pragma once

#include <memory>

#include "axgl/namespace.h"
#include "axgl/world/entity.h"
#include "axgl/opengl/model.h"
#include "axgl/opengl/texture.h"
#include "axgl/opengl/shader_program.h"

NAMESPACE_OPENGL_ENTITY

class Skybox : public world::Entity
{
  std::shared_ptr<Model> model_;

public:
  Skybox(
    std::shared_ptr<ShaderProgram> skybox_shader,
    std::shared_ptr<Texture> skybox_texture
  );
  void render(const RenderContext& context) override;
};

NAMESPACE_OPENGL_ENTITY_END
