#pragma once

#include <glm/glm.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/opengl/texture.hpp>

namespace axgl::impl::opengl
{

struct Text final
{
  axgl::ptr_t<Texture> texture;
  glm::vec2 offset{0};
};

} // namespace axgl::impl::opengl
