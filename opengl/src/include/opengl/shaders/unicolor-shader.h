#pragma once

#include "../shader.h"

namespace gl
{
    namespace shader
    {
        class UniColor : public ShaderProgram
        {
        public:
            struct Uniforms : public ShaderProgram::Uniforms
            {
                glm::mat4 mvp;
                glm::vec3 color;
            };

            struct Vertex final
            {
                glm::vec3 pos;
            };

        public:
            UniColor() : ShaderProgram(
                {
                    {3, GL_FLOAT, GL_FALSE, (void*)offsetof(Vertex, pos)}
                },
                {
                    {GL_VERTEX_SHADER, "res/shaders/debug.vs"},
                    {GL_FRAGMENT_SHADER, "res/shaders/debug.fs"}
                }
            ) {}

            void SetUniforms(const ShaderProgram::Uniforms& uniforms) override
            {
                auto set_uniforms = [this](const Uniforms& uniforms)
                {
                    SetMat4("mvp", uniforms.mvp);
                    SetVec3("color", uniforms.color);
                };
                CastUniforms<Uniforms>(uniforms, set_uniforms);
            }
        };
    }
}
