#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/light.hpp>

namespace axgl::impl::opengl::renderer
{

struct RenderContext
{
  glm::vec2 viewport{0.0f};
  glm::vec3 viewpoint{0.0f};
  glm::mat4 view_matrix{0.0f};
  glm::mat4 projection_matrix{0.0f};
  glm::mat4 projection_view_matrix{0.0f};
  std::vector<const axgl::Light*> lights;
};

} // namespace axgl::impl::opengl::renderer
