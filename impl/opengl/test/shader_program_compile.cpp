#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "gl_context.hpp"
#include <opengl/shader_program.hpp>

namespace {

const char* const VS_SOURCE
  = "#version 410 core\n"
    "layout(location = 0) in vec3 position;\n"
    "void main() { gl_Position = vec4(position, 1.0); }\n";

const char* const FS_SOURCE = "#version 410 core\n"
                              "out vec4 frag_color;\n"
                              "void main() { frag_color = vec4(1.0); }\n";
} // namespace

TEST_SUITE("opengl::ShaderSource::compile") {
  TEST_CASE("embedded shaders compile and link into a valid program") {
    const GlContext gl;
    if (!gl.available()) {
      MESSAGE("skipped: no GL context available");
      return;
    }

    const opengl::ShaderSource vs(GL_VERTEX_SHADER, VS_SOURCE);
    const opengl::ShaderSource fs(GL_FRAGMENT_SHADER, FS_SOURCE);
    const opengl::ShaderProgram program({vs, fs});

    REQUIRE(program.get_link_status() == GL_TRUE);
  }
}
