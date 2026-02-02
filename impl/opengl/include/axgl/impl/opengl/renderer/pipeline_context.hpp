#pragma once

#include <functional>

#include <axgl/common.hpp>

#include <axgl/impl/opengl/renderer/render_context.hpp>

namespace axgl::impl::opengl::renderer
{

struct PipelineContext
{
  std::vector<std::function<void(const RenderContext&)>> opaque_pass;
  std::vector<std::function<void(const RenderContext&)>> blend_pass;
};

}; // namespace axgl::impl::opengl::renderer
