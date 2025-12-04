#pragma once

#include <string_view>
#include <unordered_map>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

namespace opengl
{

class ShaderProgram final
{
public:
  struct Shader final
  {
    GLenum type;
    std::string_view source_code;

    Shader(const GLenum type, const std::string& data) : type(type), source_code(data) { }
    Shader(const GLenum type, const std::span<const uint8_t>& data) :
      type(type), source_code(reinterpret_cast<const char*>(data.data()), data.size())
    {
    }
  };

private:
  GLuint program_id_;
  std::unordered_map<std::string, GLint> uniform_locations_;

public:
  explicit ShaderProgram(const std::vector<Shader>& shaders)
  {
    program_id_ = glCreateProgram();

    std::vector<GLuint> shader_ids;
    shader_ids.reserve(shaders.size());
    for (const auto& shader : shaders)
    {
      auto shader_id = create_shader(shader);
      glAttachShader(program_id_, shader_id);
      shader_ids.push_back(shader_id);
    }

    glLinkProgram(program_id_);

    int success;
    glGetProgramiv(program_id_, GL_LINK_STATUS, &success);
    if (!success)
    {
      char log[512] = {};
      glGetProgramInfoLog(program_id_, sizeof(log), nullptr, log);
      SPDLOG_CRITICAL("Failed to link shader program: {}", log);
    }

    for (const auto shader_id : shader_ids)
      glDeleteShader(shader_id);
  }

  ShaderProgram(const ShaderProgram&) = delete;
  ShaderProgram& operator=(const ShaderProgram&) = delete;

  ShaderProgram(ShaderProgram&& other) noexcept
  {
    program_id_ = other.program_id_;
    other.program_id_ = 0;
  }
  ShaderProgram& operator=(ShaderProgram&& other) noexcept
  {
    if (this != &other)
    {
      if (program_id_ > 0)
        glDeleteProgram(program_id_);

      program_id_ = other.program_id_;
      other.program_id_ = 0;
    }
    return *this;
  }

  ~ShaderProgram()
  {
    if (program_id_ > 0)
      glDeleteProgram(program_id_);
  }

  void set_int(const std::string& name, const int value) { glUniform1i(get_uniform_location(name), value); }
  void set_bool(const std::string& name, const bool value)
  {
    glUniform1i(get_uniform_location(name), static_cast<GLint>(value));
  }
  void set_float(const std::string& name, const float value) { glUniform1f(get_uniform_location(name), value); }
  void set_vec2(const std::string& name, const glm::vec2& value)
  {
    glUniform2fv(get_uniform_location(name), 1, &value[0]);
  }
  void set_vec3(const std::string& name, const glm::vec3& value)
  {
    glUniform3fv(get_uniform_location(name), 1, &value[0]);
  }
  void set_vec4(const std::string& name, const glm::vec4& value)
  {
    glUniform4fv(get_uniform_location(name), 1, &value[0]);
  }
  void set_vec2(const std::string& name, const float x, const float y)
  {
    glUniform2f(get_uniform_location(name), x, y);
  }
  void set_vec3(const std::string& name, const float x, const float y, const float z)
  {
    glUniform3f(get_uniform_location(name), x, y, z);
  }
  void set_vec4(const std::string& name, const float x, const float y, const float z, const float w)
  {
    glUniform4f(get_uniform_location(name), x, y, z, w);
  }
  void set_mat2(const std::string& name, const glm::mat2& mat)
  {
    glUniformMatrix2fv(get_uniform_location(name), 1, GL_FALSE, &mat[0][0]);
  }
  void set_mat3(const std::string& name, const glm::mat3& mat)
  {
    glUniformMatrix3fv(get_uniform_location(name), 1, GL_FALSE, &mat[0][0]);
  }
  void set_mat4(const std::string& name, const glm::mat4& mat)
  {
    glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, &mat[0][0]);
  }

  void use_program() const { glUseProgram(program_id_); }

private:
  GLint get_uniform_location(const std::string& name)
  {
    if (!uniform_locations_.contains(name))
      uniform_locations_[name] = glGetUniformLocation(program_id_, name.c_str());

    return uniform_locations_[name];
  }

  static GLuint create_shader(const Shader& shader)
  {
    const GLuint id = glCreateShader(shader.type);

    const GLchar* code = shader.source_code.data();
    const auto size = static_cast<GLint>(shader.source_code.size());
    glShaderSource(id, 1, &code, &size);

    glCompileShader(id);

    int success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      char log[512] = {};
      glGetShaderInfoLog(id, sizeof(log), nullptr, log);
      SPDLOG_CRITICAL("Failed to compile shader: {}", log);
    }
    return id;
  }
};

} // namespace opengl
