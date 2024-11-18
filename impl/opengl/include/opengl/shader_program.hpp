#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <spdlog/spdlog.h>

namespace opengl
{

  class ShaderProgram
  {
  private:
    inline static GLuint create_shader(const char* code, GLenum type)
    {
      GLuint id = glCreateShader(type);

      glShaderSource(id, 1, &code, NULL);

      glCompileShader(id);

      int success;
      glGetShaderiv(id, GL_COMPILE_STATUS, &success);
      if (!success)
      {
        char log[512] = {};
        glGetShaderInfoLog(id, sizeof(log), NULL, log);
        SPDLOG_CRITICAL("Failed to compile shader: {}", log);
      }
      return id;
    }

  public:
    struct Shader final
    {
      GLenum type;
      std::string source_code;
    };

  private:
    GLuint program_id_;

  public:
    ShaderProgram(const std::vector<Shader>& shaders)
    {
      program_id_ = glCreateProgram();

      std::vector<GLuint> shader_ids;
      shader_ids.reserve(shaders.size());
      // attach shaders
      for (const auto& shader : shaders)
      {
        auto shader_id = create_shader(shader.source_code.c_str(), shader.type);
        glAttachShader(program_id_, shader_id);
        shader_ids.push_back(shader_id);
      }

      // link program
      glLinkProgram(program_id_);

      int success;
      glGetProgramiv(program_id_, GL_LINK_STATUS, &success);
      if (!success)
      {
        char log[512] = {};
        glGetProgramInfoLog(program_id_, sizeof(log), NULL, log);
        SPDLOG_CRITICAL("Failed to link shader program: {}", log);
      }

      // delete shaders after linking program
      for (auto shader_id : shader_ids)
        glDeleteShader(shader_id);
    }

    virtual ~ShaderProgram()
    {
      glDeleteProgram(program_id_);
    }

  public:
    void set_bool(const std::string& name, bool value) const { glUniform1i(glGetUniformLocation(program_id_, name.c_str()), (int)value); }
    void set_int(const std::string& name, int value) const { glUniform1i(glGetUniformLocation(program_id_, name.c_str()), value); }
    void set_float(const std::string& name, float value) const { glUniform1f(glGetUniformLocation(program_id_, name.c_str()), value); }
    void set_vec2(const std::string& name, const glm::vec2& value) const { glUniform2fv(glGetUniformLocation(program_id_, name.c_str()), 1, &value[0]); }
    void set_vec2(const std::string& name, float x, float y) const { glUniform2f(glGetUniformLocation(program_id_, name.c_str()), x, y); }
    void set_vec3(const std::string& name, const glm::vec3& value) const { glUniform3fv(glGetUniformLocation(program_id_, name.c_str()), 1, &value[0]); }
    void set_vec3(const std::string& name, float x, float y, float z) const { glUniform3f(glGetUniformLocation(program_id_, name.c_str()), x, y, z); }
    void set_vec4(const std::string& name, const glm::vec4& value) const { glUniform4fv(glGetUniformLocation(program_id_, name.c_str()), 1, &value[0]); }
    void set_vec4(const std::string& name, float x, float y, float z, float w) const { glUniform4f(glGetUniformLocation(program_id_, name.c_str()), x, y, z, w); }
    void set_mat2(const std::string& name, const glm::mat2& mat) const { glUniformMatrix2fv(glGetUniformLocation(program_id_, name.c_str()), 1, GL_FALSE, &mat[0][0]); }
    void set_mat3(const std::string& name, const glm::mat3& mat) const { glUniformMatrix3fv(glGetUniformLocation(program_id_, name.c_str()), 1, GL_FALSE, &mat[0][0]); }
    void set_mat4(const std::string& name, const glm::mat4& mat) const { glUniformMatrix4fv(glGetUniformLocation(program_id_, name.c_str()), 1, GL_FALSE, &mat[0][0]); }

    void use_program() const { glUseProgram(program_id_); }
  };

}
