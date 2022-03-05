#include "pch.h"
#include "include/opengl/shader.h"

namespace gl
{
    GLuint ShaderProgram::CompileShader(const ShaderFile& file)
    {
        auto source_code = util::File::ReadTextFile(file.path);
        const char* source_c_str = source_code.c_str();

        GLuint id = glCreateShader(file.type);

        glShaderSource(id, 1, &source_c_str, NULL);

        glCompileShader(id);

        int success;
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char log[512] = {};
            glGetShaderInfoLog(id, sizeof(log), NULL, log);
            std::cout << log << std::endl;
            throw std::runtime_error("Failed to compile shader.");
        }
        return id;
    }

    ShaderProgram::ShaderProgram(
        const std::vector<Attribute>& attributes,
        const std::vector<ShaderFile>& files
    )
        : attributes_(attributes)
    {
        program_id_ = glCreateProgram();

        const size_t count = files.size();
        std::vector<GLuint> shaders(count);

        for (size_t i = 0; i < count; ++i)
        {
            shaders.at(i) = CompileShader(files.at(i));
            glAttachShader(program_id_, shaders.at(i));
        }

        glLinkProgram(program_id_);

        int success = GL_FALSE;
        glGetProgramiv(program_id_, GL_LINK_STATUS, &success);
        if (success == GL_FALSE)
        {
            char log[512] = {};
            glGetProgramInfoLog(program_id_, sizeof(log), NULL, log);
            std::cout << log << std::endl;
            throw std::runtime_error("Failed to link shader program.");
        }

        for (const auto& shader : shaders)
            glDeleteShader(shader);
    }

    ShaderProgram::~ShaderProgram()
    {
        glDeleteProgram(program_id_);
    }

    void ShaderProgram::Use(const Uniforms& uniforms)
    {
        glUseProgram(program_id_);
        SetUniforms(uniforms);
    }

    void ShaderProgram::SetBool(const std::string& name, bool value) const { glUniform1i(glGetUniformLocation(program_id_, name.c_str()), (int)value); }
    void ShaderProgram::SetInt(const std::string& name, int value) const { glUniform1i(glGetUniformLocation(program_id_, name.c_str()), value); }
    void ShaderProgram::SetFloat(const std::string& name, float value) const { glUniform1f(glGetUniformLocation(program_id_, name.c_str()), value); }
    void ShaderProgram::SetVec2(const std::string& name, const glm::vec2& value) const { glUniform2fv(glGetUniformLocation(program_id_, name.c_str()), 1, &value[0]); }
    void ShaderProgram::SetVec2(const std::string& name, float x, float y) const { glUniform2f(glGetUniformLocation(program_id_, name.c_str()), x, y); }
    void ShaderProgram::SetVec3(const std::string& name, const glm::vec3& value) const { glUniform3fv(glGetUniformLocation(program_id_, name.c_str()), 1, &value[0]); }
    void ShaderProgram::SetVec3(const std::string& name, float x, float y, float z) const { glUniform3f(glGetUniformLocation(program_id_, name.c_str()), x, y, z); }
    void ShaderProgram::SetVec4(const std::string& name, const glm::vec4& value) const { glUniform4fv(glGetUniformLocation(program_id_, name.c_str()), 1, &value[0]); }
    void ShaderProgram::SetVec4(const std::string& name, float x, float y, float z, float w) const { glUniform4f(glGetUniformLocation(program_id_, name.c_str()), x, y, z, w); }
    void ShaderProgram::SetMat2(const std::string& name, const glm::mat2& mat) const { glUniformMatrix2fv(glGetUniformLocation(program_id_, name.c_str()), 1, GL_FALSE, &mat[0][0]); }
    void ShaderProgram::SetMat3(const std::string& name, const glm::mat3& mat) const { glUniformMatrix3fv(glGetUniformLocation(program_id_, name.c_str()), 1, GL_FALSE, &mat[0][0]); }
    void ShaderProgram::SetMat4(const std::string& name, const glm::mat4& mat) const { glUniformMatrix4fv(glGetUniformLocation(program_id_, name.c_str()), 1, GL_FALSE, &mat[0][0]); }

}
