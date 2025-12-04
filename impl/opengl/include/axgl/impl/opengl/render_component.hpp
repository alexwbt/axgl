#pragma once

#include <functional>
#include <vector>

#include <axgl/interface/camera.hpp>
#include <axgl/interface/entity.hpp>
#include <axgl/interface/light.hpp>

namespace axgl::impl::opengl
{

class RenderComponent
{
public:
  struct Context
  {
    const axgl::Camera* camera;
    std::vector<const axgl::Light*> lights;
    std::vector<std::function<void(const Context&)>> opaque_pass;
    std::vector<std::function<void(const Context&)>> blend_pass;
  };

  virtual ~RenderComponent() = default;
  virtual void collect(const axgl::Entity& entity) = 0;
  virtual void build(Context& context) = 0;
  virtual std::uint64_t get_id() = 0;
};

} // namespace axgl::impl::opengl
