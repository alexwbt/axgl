#pragma once

#include <functional>

#include <axgl/common.hpp>
#include <axgl/interface/entity.hpp>
#include <axgl/interface/light.hpp>

namespace axgl::impl::opengl
{

class RenderComponent
{
public:
  struct RenderContext;
  struct Context
  {
    std::vector<const axgl::Light*> lights;
    std::vector<std::function<void(const RenderContext&)>> opaque_pass;
    std::vector<std::function<void(const RenderContext&)>> blend_pass;
  };
  struct RenderContext : Context
  {
    glm::vec2 viewport{0.0f};
    glm::vec3 position_of_view{0.0f};
    glm::mat4 view_matrix{0.0f};
    glm::mat4 projection_matrix{0.0f};
    glm::mat4 projection_view_matrix{0.0f};
  };

  virtual ~RenderComponent() = default;
  virtual void gather_instances(const axgl::Entity& entity) = 0;
  virtual void submit_render_function(Context& context) = 0;
  virtual std::uint64_t get_id() = 0;
};

} // namespace axgl::impl::opengl
