#pragma once

#include "shader.h"
#include "texture.h"

namespace gl
{
    class Model
    {
    private:
        enum class Type
        {
            kVertex,
            kElement
        };

        std::shared_ptr<ShaderProgram> shader_;
        std::vector<std::shared_ptr<Texture>> textures_;

        Type type_;

        GLuint vao_id_ = 0;
        GLuint vbo_id_ = 0;
        GLuint ebo_id_ = 0;

        GLsizei vertices_size_ = 0;
        GLsizei indices_size_ = 0;

        GLenum mode_;

    public:
        template <typename VertexType>
        Model(
            const std::vector<VertexType>& vertices,
            std::shared_ptr<ShaderProgram> shader,
            const std::vector<std::shared_ptr<Texture>>& textures,
            GLenum mode = GL_TRIANGLES
        ) :
            type_(Type::kVertex),
            vertices_size_(static_cast<GLsizei>(vertices.size())),
            shader_(std::move(shader)),
            textures_(textures),
            mode_(mode)
        {
            glGenVertexArrays(1, &vao_id_);
            glGenBuffers(1, &vbo_id_);

            glBindVertexArray(vao_id_);

            glBindBuffer(GL_ARRAY_BUFFER, vbo_id_);
            glBufferData(GL_ARRAY_BUFFER, vertices_size_ * sizeof(VertexType), vertices.data(), GL_STATIC_DRAW);

            const auto attributes = shader->attributes();
            for (size_t i = 0; i < attributes.size(); ++i)
            {
                const auto& attr = attributes.at(i);
                glEnableVertexAttribArray(i);
                glVertexAttribPointer(i, attr.size, attr.type, attr.normalized, sizeof(VertexType), attr.offset);
            }

            glBindVertexArray(0);
        }

        template <typename VertexType>
        Model(
            const std::vector<VertexType>& vertices,
            const std::vector<uint32_t>& indices,
            std::shared_ptr<ShaderProgram> shader,
            const std::vector<std::shared_ptr<Texture>>& textures,
            GLenum mode = GL_TRIANGLES
        ) :
            shader_(shader),
            type_(Type::kElement),
            vertices_size_(static_cast<GLsizei>(vertices.size())),
            indices_size_(static_cast<GLsizei>(indices.size())),
            textures_(textures),
            mode_(mode)
        {
            glGenVertexArrays(1, &vao_id_);
            glGenBuffers(1, &vbo_id_);
            glGenBuffers(1, &ebo_id_);

            glBindVertexArray(vao_id_);

            glBindBuffer(GL_ARRAY_BUFFER, vbo_id_);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexType), vertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id_);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

            const auto attributes = shader->attributes();
            for (size_t i = 0; i < attributes.size(); ++i)
            {
                const auto& attr = attributes.at(i);
                glEnableVertexAttribArray(i);
                glVertexAttribPointer(i, attr.size, attr.type, attr.normalized, sizeof(VertexType), attr.offset);
            }

            glBindVertexArray(0);
        }

        ~Model();

        void Render(const ShaderProgram::Uniforms& uniforms);
    };
}
