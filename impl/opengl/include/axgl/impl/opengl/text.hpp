#pragma once

#include <memory>

#include <glm/glm.hpp>

#include <axgl/impl/opengl/texture.hpp>

namespace axgl::impl::opengl
{

struct Text final
{
  std::shared_ptr<Texture> texture;
  glm::vec2 offset{0};
};

} // namespace axgl::impl::opengl
