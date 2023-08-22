#pragma once

#include <memory>
#include <vector>

#include "glad/glad.h"
#include "axgl/namespace.h"
#include "axgl/opengl/texture.h"
#include "axgl/opengl/shader_program.h"

NAMESPACE_OPENGL

struct Vertex1
{
  glm::vec3 pos;

  static void enable_attributes();
};

struct Vertex2
{
  glm::vec3 pos;
  glm::vec3 color;

  static void enable_attributes();
};

struct Vertex3
{
  glm::vec3 pos;
  glm::vec3 normal;
  glm::vec3 color;

  static void enable_attributes();
};

struct Vertex7
{
  glm::vec3 pos;
  glm::vec3 normal;
  glm::vec2 uv;
  glm::vec3 tangent;
  glm::vec3 bitangent;
  int bones[4];
  float bone_weights[4];

  static void enable_attributes();
};

class Mesh
{
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

  std::vector<std::shared_ptr<Texture>> textures_;

public:
  template <typename VertexType>
  Mesh(
    const std::vector<VertexType>& vertices,
    GLenum mode = GL_TRIANGLES
  ) :
    mode_(mode),
    type_(Type::kVertex),
    vertices_size_((GLsizei)vertices.size())
  {
    glGenVertexArrays(1, &vao_id_);
    glGenBuffers(1, &vbo_id_);

    glBindVertexArray(vao_id_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_id_);
    glBufferData(GL_ARRAY_BUFFER, vertices_size_ * sizeof(VertexType), vertices.data(), GL_STATIC_DRAW);

    VertexType::enable_attributes();

    glBindVertexArray(0);
  }

  template <typename VertexType>
  Mesh(
    const std::vector<VertexType>& vertices,
    const std::vector<uint32_t>& indices,
    GLenum mode = GL_TRIANGLES
  ) :
    mode_(mode),
    type_(Type::kElement),
    vertices_size_((GLsizei)vertices.size()),
    indices_size_((GLsizei)indices.size())
  {
    glGenVertexArrays(1, &vao_id_);
    glGenBuffers(1, &vbo_id_);
    glGenBuffers(1, &ebo_id_);

    glBindVertexArray(vao_id_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_id_);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexType), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    VertexType::enable_attributes();

    glBindVertexArray(0);
  }

  ~Mesh();

  void add_texture(std::shared_ptr<Texture> texture);
  void render(std::shared_ptr<ShaderProgram> shader);
};

class Model
{
public:
  static std::shared_ptr<Model> load_model(const std::string& path);

private:
  std::vector<std::shared_ptr<Mesh>> meshes_;

public:
  glm::vec3 scale{ 1.0f };
  glm::vec3 rotation{ 0.0f };
  glm::vec3 translation{ 0.0f };

public:
  glm::mat4 model_matrix();

  void add_mesh(std::shared_ptr<Mesh> mesh);
  void render(std::shared_ptr<ShaderProgram> shader);
};

NAMESPACE_OPENGL_END
