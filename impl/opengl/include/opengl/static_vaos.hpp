#pragma once

#include <array>
#include <glm/glm.hpp>
#include <opengl/vertex_array_object.hpp>

namespace opengl
{

  class StaticVAOs {
  public:
    static const StaticVAOs& instance() {
      static StaticVAOs instance_;
      return instance_;
    }

    StaticVAOs(const StaticVAOs&) = delete;
    StaticVAOs& operator=(const StaticVAOs&) = delete;
    StaticVAOs(const StaticVAOs&&) = delete;
    StaticVAOs& operator=(const StaticVAOs&&) = delete;

    const VertexArrayObject& quad() const { return quad_; }

  private:
    VertexArrayObject quad_;

    StaticVAOs()
    {
      init_quad();
    }

    void init_quad()
    {
      std::array<glm::vec4, 6> vertices{
        glm::vec4{ 0, 0, 0, 1 },
        glm::vec4{ 0, 1, 0, 0 },
        glm::vec4{ 1, 0, 1, 1 },
        glm::vec4{ 1, 0, 1, 1 },
        glm::vec4{ 0, 1, 0, 0 },
        glm::vec4{ 1, 1, 1, 0 }
      };
      std::array<VertexAttribute, 2> attributes{
        opengl::VertexAttribute{ 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), 0 },
        opengl::VertexAttribute{ 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)offsetof(glm::vec4, z) }
      };
      quad_.create_vertex_buffer<glm::vec4>(vertices, attributes, 0);
    }
  };

}
