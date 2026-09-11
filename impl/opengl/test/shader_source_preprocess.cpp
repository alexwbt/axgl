#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <opengl/shader_source.hpp>

#include <glad/glad.h>
#include <string>

#ifdef AXGL_DEBUG
  #define AXGL_DEBUG_LINE "#define AXGL_DEBUG\n"
#else
  #define AXGL_DEBUG_LINE ""
#endif

TEST_SUITE("opengl::ShaderSource::preprocess")
{
  TEST_CASE("prepend is injected after the first line")
  {
    const std::string source = "#version 410 core\nvoid main() {}\n";
    const std::string prepend = "#define FOO 1\n";

    const opengl::ShaderSource s(GL_FRAGMENT_SHADER, source, prepend);
    const auto result = s.preprocess();

    const std::string expected
      = "#version 410 core\n\n" AXGL_DEBUG_LINE "#define FOO 1\n"
        "void main() {}\n";
    CHECK(result == expected);
  }

  TEST_CASE("first line is preserved verbatim including trailing content")
  {
    const std::string source = "#version 410 core\nbody";
    const opengl::ShaderSource s(GL_FRAGMENT_SHADER, source);
    const auto result = s.preprocess();

    CHECK(result.starts_with("#version 410 core\n"));
    CHECK(result.find("body") != std::string::npos);
  }

  TEST_CASE("empty prepend leaves only the version + debug define")
  {
    const std::string source = "#version 410 core\ncode";
    const opengl::ShaderSource s(GL_VERTEX_SHADER, source, "");
    const auto result = s.preprocess();

    const std::string expected = "#version 410 core\n\n" AXGL_DEBUG_LINE "code";
    CHECK(result == expected);
  }

  TEST_CASE("multi-line prepend is fully injected")
  {
    const std::string source = "#version 410 core\nmain\n";
    const std::string prepend = "#define A 1\n"
                                "#define B 2\n"
                                "#define C 3\n";

    const opengl::ShaderSource s(GL_FRAGMENT_SHADER, source, prepend);
    const auto result = s.preprocess();

    const std::string expected
      = "#version 410 core\n\n" AXGL_DEBUG_LINE "#define A 1\n"
        "#define B 2\n"
        "#define C 3\n"
        "main\n";
    CHECK(result == expected);
  }

  TEST_CASE("prepend appears before the shader body but after #version")
  {
    const std::string source = "#version 410 core\nBODY_MARKER\n";
    const std::string prepend = "PREPEND_MARKER\n";

    const opengl::ShaderSource s(GL_FRAGMENT_SHADER, source, prepend);
    const auto result = s.preprocess();

    const auto version_pos = result.find("#version 410 core");
    const auto prepend_pos = result.find("PREPEND_MARKER");
    const auto body_pos = result.find("BODY_MARKER");

    CHECK(version_pos != std::string::npos);
    CHECK(prepend_pos != std::string::npos);
    CHECK(body_pos != std::string::npos);
    CHECK(version_pos < prepend_pos);
    CHECK(prepend_pos < body_pos);
  }

  TEST_CASE("source without a newline injects prepend after the whole string")
  {
    const std::string source = "#version 410 core";
    const std::string prepend = "#define X\n";

    const opengl::ShaderSource s(GL_FRAGMENT_SHADER, source, prepend);
    const auto result = s.preprocess();

    const std::string expected
      = "#version 410 core\n\n" AXGL_DEBUG_LINE "#define X\n";
    CHECK(result == expected);
  }

  TEST_CASE("byte-span constructor produces the same output as string")
  {
    const std::string source = "#version 410 core\nbody\n";
    const std::string prepend = "#define P 1\n";

    const std::span<const uint8_t> bytes(
      reinterpret_cast<const uint8_t*>(source.data()), source.size()
    );

    const opengl::ShaderSource from_string(GL_FRAGMENT_SHADER, source, prepend);
    const opengl::ShaderSource from_bytes(GL_FRAGMENT_SHADER, bytes, prepend);

    CHECK(from_string.preprocess() == from_bytes.preprocess());
  }

  TEST_CASE("include directive is resolved from embedded resources")
  {
    const std::string source = "#version 410 core\n"
                               "#include \"shader/color.fs\"\n"
                               "void main() {}\n";

    const opengl::ShaderSource s(GL_FRAGMENT_SHADER, source);
    const auto result = s.preprocess();

    CHECK(result.find("#include") == std::string::npos);

    const auto included = axgl_opengl_impl_res::get("shader/color.fs");
    const std::string included_str(
      reinterpret_cast<const char*>(included.data()), included.size()
    );
    CHECK(result.find(included_str) != std::string::npos);
  }

  TEST_CASE("prepend and include combine in a single pass")
  {
    const std::string source = "#version 410 core\n"
                               "#include \"shader/color.fs\"\n"
                               "void main() {}\n";
    const std::string prepend = "#define COMBINED 1\n";

    const opengl::ShaderSource s(GL_FRAGMENT_SHADER, source, prepend);
    const auto result = s.preprocess();

    CHECK(result.find("#include") == std::string::npos);
    CHECK(result.find("#define COMBINED 1") != std::string::npos);

    const auto version_pos = result.find("#version 410 core");
    const auto prepend_pos = result.find("#define COMBINED 1");
    CHECK(version_pos < prepend_pos);
  }
}
