#pragma once

#include <axgl/common.hpp>
#include <glad/glad.h>

#include <axgl_opengl_impl/res.hpp>

namespace opengl
{

class ShaderSource final
{
  GLenum type_;
  std::string source_code_;
  std::string prepend_;

public:
  ShaderSource(
    const GLenum type, std::string source_code, std::string prepend = ""
  ) :
    type_(type),
    source_code_(std::move(source_code)),
    prepend_(std::move(prepend))
  {
  }

  ShaderSource(
    const GLenum type,
    const std::span<const uint8_t>& source_code,
    std::string prepend = ""
  ) :
    type_(type),
    source_code_(
      reinterpret_cast<const char*>(source_code.data()), source_code.size()
    ),
    prepend_(std::move(prepend))
  {
  }

  [[nodiscard]] std::string preprocess() const
  {
    // injects prepend_ after the first line, then appends the rest of `source`.
    const auto first_newline = source_code_.find('\n');
    std::string shader_code;
    shader_code += source_code_.substr(0, first_newline + 1) + '\n';
#ifdef AXGL_DEBUG
    shader_code += "#define AXGL_DEBUG\n";
#endif
    shader_code += prepend_;
    shader_code += source_code_.substr(first_newline + 1);

    // handle includes
    auto include = shader_code.find("#include");
    while (include != std::string::npos)
    {
      const auto quote_begin = shader_code.find('"', include);
      const auto quote_end = shader_code.find('"', quote_begin);

      const auto include_path
        = shader_code.substr(quote_begin + 1, quote_end - quote_begin - 1);
#ifdef AXGL_DEBUG
      if (!axgl_opengl_impl_res::data.contains(include_path))
        AXGL_LOG_ERROR("Shader include resource not round: ", include_path);
#endif
      const auto include_content = axgl_opengl_impl_res::get(include_path);
      shader_code.replace(
        include, quote_end - include,
        reinterpret_cast<const char*>(include_content.data())
      );

      include
        = shader_code.find("#include", quote_end + include_content.size());
    }

    return shader_code;
  }

  [[nodiscard]] GLuint compile(GLuint program_id) const
  {
    const auto shader_code = preprocess();
    const auto* code = shader_code.data();
    const auto size = util::narrow<GLint>(shader_code.size());

    const GLuint id = glCreateShader(type_);
    glShaderSource(program_id, 1, &code, &size);
    glCompileShader(id);

    int success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      char log[512] = {};
      glGetShaderInfoLog(id, sizeof(log), nullptr, log);
      AXGL_LOG_ERROR("Failed to compile shader: {}", log);
      AXGL_LOG_DEBUG("\n{}", shader_code);
    }
    return id;
  }
};

} // namespace opengl
