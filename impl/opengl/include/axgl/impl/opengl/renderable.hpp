#pragma once

#include <axgl/interface/entity.hpp>
#include <axgl/interface/renderer.hpp>>

namespace axgl::impl::opengl
{

class Renderable
{
public:
  virtual ~Renderable() = default;
  virtual void render(const axgl::Renderer::Context& context, const axgl::Entity& entity) const = 0;
  [[nodiscard]] virtual bool enabled_blend() = 0;
};

} // namespace axgl::impl::opengl
