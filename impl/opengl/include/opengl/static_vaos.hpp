#pragma once

#include <array>
#include <glm/glm.hpp>
#include <memory>
#include <opengl/vertex_array_object.hpp>

namespace opengl
{

class StaticVAOs
{
public:
  static const StaticVAOs& instance()
  {
    static StaticVAOs instance_;
    return instance_;
  }

  StaticVAOs(const StaticVAOs&) = delete;
  StaticVAOs& operator=(const StaticVAOs&) = delete;
  StaticVAOs(const StaticVAOs&&) = delete;
  StaticVAOs& operator=(const StaticVAOs&&) = delete;

  [[nodiscard]] std::shared_ptr<VertexArrayObject> quad() const { return quad_; }

private:
  std::shared_ptr<VertexArrayObject> quad_;

  StaticVAOs() { init_quad(); }

  void init_quad()
  {
    std::array vertices{
      glm::vec2{0, 0}, glm::vec2{0, 1}, glm::vec2{1, 0}, glm::vec2{1, 0}, glm::vec2{0, 1}, glm::vec2{1, 1}};
    std::array attributes{VertexAttribute{2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0},
      VertexAttribute{2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0}};
    quad_ = std::make_shared<VertexArrayObject>();
    quad_->create_vertex_buffer<glm::vec2>(vertices, attributes, 0);
  }
};

} // namespace opengl
