#pragma once

#include <memory>

#include <glm/glm.hpp>

#include <axgl/impl/opengl/texture.hpp>

namespace axgl::impl
{

struct OpenglText final
{
  std::shared_ptr<OpenglTexture> texture;
  glm::vec2 offset{0};
};

} // namespace axgl::impl
