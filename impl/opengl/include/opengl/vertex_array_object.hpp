#pragma once

#include "opengl/buffer_object.hpp"

namespace opengl
{

  class VertexArrayObject final
  {
  private:
    GLuint id_;

  public:
    VertexArrayObject()
    {
      glGenVertexArrays(1, &id_);
    }

    ~VertexArrayObject()
    {
      glDeleteVertexArrays(1, &id_);
    }
  };

}
