#pragma once

#include <opengl/shader_program.hpp>
#include <axgl_opengl_impl/res.hpp>

namespace opengl
{

  class StaticShaders {
  public:
    static StaticShaders& instance() {
      static StaticShaders instance_;
      return instance_;
    }

    StaticShaders(const StaticShaders&) = delete;
    StaticShaders& operator=(const StaticShaders&) = delete;
    StaticShaders(const StaticShaders&&) = delete;
    StaticShaders& operator=(const StaticShaders&&) = delete;

    ShaderProgram& mesh_2d() { return mesh_2d_; }
    ShaderProgram& mesh_3d() { return mesh_3d_; }
    ShaderProgram& text() { return text_; }

  private:
    ShaderProgram mesh_2d_{ {
      { GL_VERTEX_SHADER, axgl_opengl_impl_res::get("shader/mesh2d.vs") },
      { GL_FRAGMENT_SHADER, axgl_opengl_impl_res::get("shader/mesh2d.fs") }
    } };
    ShaderProgram mesh_3d_{ {
      { GL_VERTEX_SHADER, axgl_opengl_impl_res::get("shader/mesh3d.vs") },
      { GL_FRAGMENT_SHADER, axgl_opengl_impl_res::get("shader/mesh3d.fs") }
    } };
    ShaderProgram text_{ {
      { GL_VERTEX_SHADER, axgl_opengl_impl_res::get("shader/mesh2d.vs") },
      { GL_FRAGMENT_SHADER, axgl_opengl_impl_res::get("shader/text.fs") }
    } };

    StaticShaders() {}
  };

}
