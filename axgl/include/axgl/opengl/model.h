#include <memory>

#include "glad/glad.h"

#include "axgl/namespace.h"

NAMESPACE_OPENGL

class Model
{
private:
  enum class Type
  {
    kVertex,
    kElement
  };

  std::shared_ptr<ShaderProgram> shader_;
  std::shared_ptr<std::vector<std::shared_ptr<Texture>>> textures_;

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

    shader_->EnableAttributes();

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

    shader_->EnableAttributes();

    glBindVertexArray(0);
  }

  ~Model()
  {
    if (vao_id_ > 0) glDeleteVertexArrays(1, &vao_id_);
    if (vbo_id_ > 0) glDeleteBuffers(1, &vbo_id_);
    if (ebo_id_ > 0) glDeleteBuffers(1, &ebo_id_);
  }

  void Render(const ShaderProgram::Uniforms& uniforms)
  {
    shader_->Use(uniforms);

    if (textures_)
      for (int i = 0; i < textures_->size(); i++)
        textures_->at(i)->Use(i);

    glBindVertexArray(vao_id_);
    switch (type_)
    {
    case Type::kVertex:
      glDrawArrays(mode_, 0, vertices_size_);
      break;
    case Type::kElement:
      glDrawElements(mode_, indices_size_, GL_UNSIGNED_INT, 0);
      break;
    }
  }
};

NAMESPACE_OPENGL_END
