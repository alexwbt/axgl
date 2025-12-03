#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/entity.hpp>
#include <axgl/interface/renderer.hpp>

namespace axgl
{

class Renderable
{
public:
  virtual ~Renderable() = default;
  virtual void render(const axgl::Renderer::Context& context, const axgl::Entity& entity) = 0;
};

} // namespace axgl
