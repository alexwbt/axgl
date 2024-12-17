#pragma once

#include <axgl/namespace.hpp>

#include <assimp/scene.h>

NAMESPACE_AXGL_IMPL

class AssimpTexture : public axgl::interface::Texture
{
public:
  virtual void load_texture(aiTexture* texture) = 0;
};

NAMESPACE_AXGL_IMPL_END
