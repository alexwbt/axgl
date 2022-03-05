#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <stdexcept>
#include <functional>

namespace gl
{
    class ShaderProgram
    {
    public:
        struct ShaderFile final
        {
            GLuint type;
            std::string path;
        };

        struct Attribute final
        {
            GLint size;
            GLenum type;
            GLboolean normalized;
            const void* offset;
        };

        struct Uniforms
        {
            virtual ~Uniforms() {}
        };

    private:
        static GLuint CompileShader(const ShaderFile& file);

    private:
        GLuint program_id_;

        std::vector<Attribute> attributes_;

    public:
        ShaderProgram(
            const std::vector<Attribute>& attributes,
            const std::vector<ShaderFile>& files
        );
        virtual ~ShaderProgram();

        const std::vector<Attribute> attributes() { return attributes_; }

        void Use(const Uniforms& uniform);

        virtual void SetUniforms(const Uniforms& uniforms) = 0;

        template <typename UniformsType>
        void CastUniforms(
            const Uniforms& uniforms,
            std::function<void(const UniformsType&)> set_uniforms
        ) {
            try
            {
                const auto& data = dynamic_cast<const UniformsType&>(uniforms);
                set_uniforms(data);
            }
            catch (const std::bad_cast& exception)
            {
                std::cout << "Failed to cast debug shader uniform." << std::endl;
                std::cout << exception.what() << std::endl;
            }
        }

    protected:
        void SetBool(const std::string& name, bool value) const;
        void SetInt(const std::string& name, int value) const;
        void SetFloat(const std::string& name, float value) const;
        void SetVec2(const std::string& name, const glm::vec2& value) const;
        void SetVec2(const std::string& name, float x, float y) const;
        void SetVec3(const std::string& name, const glm::vec3& value) const;
        void SetVec3(const std::string& name, float x, float y, float z) const;
        void SetVec4(const std::string& name, const glm::vec4& value) const;
        void SetVec4(const std::string& name, float x, float y, float z, float w) const;
        void SetMat2(const std::string& name, const glm::mat2& value) const;
        void SetMat3(const std::string& name, const glm::mat3& value) const;
        void SetMat4(const std::string& name, const glm::mat4& value) const;

    };
}
