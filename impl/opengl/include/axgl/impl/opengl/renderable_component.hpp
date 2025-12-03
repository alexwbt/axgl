#pragma once

#include <functional>
#include <vector>

#include <axgl/interface/camera.hpp>
#include <axgl/interface/entity.hpp>
#include <axgl/interface/light.hpp>

namespace axgl::impl::opengl
{

class RenderableComponent
{
public:
  using RenderFunction = std::function<void()>;

  struct Context
  {
    const axgl::Camera* camera;
    std::vector<const axgl::Light*> lights;
    std::vector<RenderFunction> opaque_pass;
    std::vector<RenderFunction> blend_pass;
  };

  virtual ~RenderableComponent() = default;
  virtual void collect_renderable(const axgl::Entity& entity) = 0;
  virtual void build_renderable(const Context& context) = 0;
  [[nodiscard]] virtual bool enabled_blend() = 0;
};

} // namespace axgl::impl::opengl
