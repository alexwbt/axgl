#pragma once

#include <memory>
#include <vector>

#include "glad/glad.h"
#include "axgl/namespace.h"
#include "axgl/opengl/texture.h"
#include "axgl/opengl/shader_program.h"

NAMESPACE_OPENGL

class Model
{
private:
  enum class Type
  {
    kVertex,
    kElement
  };

  Type type_;
  GLenum mode_;

  GLuint vao_id_ = 0;
  GLuint vbo_id_ = 0;
  GLuint ebo_id_ = 0;

  GLsizei vertices_size_ = 0;
  GLsizei indices_size_ = 0;

  std::shared_ptr<ShaderProgram> shader_;
  std::shared_ptr<std::vector<std::shared_ptr<Texture>>> textures_;

public:
  template <typename VertexType>
  Model(
    const std::vector<VertexType>& vertices,
    std::shared_ptr<ShaderProgram> shader,
    std::shared_ptr<std::vector<std::shared_ptr<Texture>>> textures,
    GLenum mode = GL_TRIANGLES
  ) :
    type_(Type::kVertex),
    vertices_size_((GLsizei)vertices.size()),
    shader_(std::move(shader)),
    textures_(std::move(textures)),
    mode_(mode)
  {
    glGenVertexArrays(1, &vao_id_);
    glGenBuffers(1, &vbo_id_);

    glBindVertexArray(vao_id_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_id_);
    glBufferData(GL_ARRAY_BUFFER, vertices_size_ * sizeof(VertexType), vertices.data(), GL_STATIC_DRAW);

    shader_->enable_attributes();

    glBindVertexArray(0);
  }

  template <typename VertexType>
  Model(
    const std::vector<VertexType>& vertices,
    const std::vector<uint32_t>& indices,
    std::shared_ptr<ShaderProgram> shader,
    std::shared_ptr<std::vector<std::shared_ptr<Texture>>> textures,
    GLenum mode = GL_TRIANGLES
  ) :
    shader_(shader),
    type_(Type::kElement),
    vertices_size_((GLsizei)vertices.size()),
    indices_size_((GLsizei)indices.size()),
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

    shader_->enable_attributes();

    glBindVertexArray(0);
  }

  ~Model();

  void render(const ShaderProgram::Uniforms& uniforms);
};

NAMESPACE_OPENGL_END
