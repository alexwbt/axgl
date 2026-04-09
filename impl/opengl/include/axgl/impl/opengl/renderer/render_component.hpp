#pragma once

#include <axgl/impl/opengl/renderer/render_context.hpp>

namespace axgl::impl::opengl::renderer
{

class RenderComponent
{
public:
  virtual ~RenderComponent() = default;
  virtual void gather_instances(const glm::mat4& transform_matrix) = 0;
  virtual void submit_render_function(PipelineContext& context) = 0;
  virtual std::uint64_t get_id() = 0;
};

} // namespace axgl::impl::opengl::renderer
