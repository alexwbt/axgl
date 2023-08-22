#include "axgl/opengl/model.h"

#include <spdlog/spdlog.h>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "axgl/opengl/static_resource.h"

NAMESPACE_OPENGL

void Vertex1::enable_attributes()
{
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex1), (void*)offsetof(Vertex1, pos));
}

void Vertex2::enable_attributes()
{
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2), (void*)offsetof(Vertex2, pos));

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2), (void*)offsetof(Vertex2, color));
}

void Vertex3::enable_attributes()
{
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3), (void*)offsetof(Vertex3, pos));

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3), (void*)offsetof(Vertex3, normal));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3), (void*)offsetof(Vertex3, color));
}

void Vertex7::enable_attributes()
{
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex7), (void*)offsetof(Vertex7, pos));
  // vertex normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex7), (void*)offsetof(Vertex7, normal));
  // vertex texture coords
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex7), (void*)offsetof(Vertex7, uv));
  // vertex tangent
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex7), (void*)offsetof(Vertex7, tangent));
  // vertex bitangent
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex7), (void*)offsetof(Vertex7, bitangent));
  // ids
  glEnableVertexAttribArray(5);
  glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex7), (void*)offsetof(Vertex7, bones));
  // weights
  glEnableVertexAttribArray(6);
  glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex7), (void*)offsetof(Vertex7, bone_weights));
}

Mesh::~Mesh()
{
  if (vao_id_ > 0) glDeleteVertexArrays(1, &vao_id_);
  if (vbo_id_ > 0) glDeleteBuffers(1, &vbo_id_);
  if (ebo_id_ > 0) glDeleteBuffers(1, &ebo_id_);
}

void Mesh::add_texture(std::shared_ptr<Texture> texture)
{
  textures_.push_back(std::move(texture));
}

void Mesh::render(std::shared_ptr<ShaderProgram> shader)
{
  int diffuse = 1;
  int specular = 1;
  int normal = 1;
  int height = 1;
  int unknown = 1;
  for (int i = 0; i < textures_.size(); ++i)
  {
    const auto& texture = textures_.at(i);
    switch (texture->texture_type())
    {
    case Texture::kDiffuse:
      texture->use(shader, i, "texture_diffuse_" + std::to_string(diffuse++));
      break;
    case Texture::kSpecular:
      texture->use(shader, i, "texture_specular_" + std::to_string(specular++));
      break;
    case Texture::kNormal:
      texture->use(shader, i, "texture_normal_" + std::to_string(normal++));
      break;
    case Texture::kHeight:
      texture->use(shader, i, "texture_height_" + std::to_string(height++));
      break;
    default:
      texture->use(shader, i, "texture_" + std::to_string(unknown++));
    }
  }

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

glm::mat4 Model::model_matrix()
{
  glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), translation);
  glm::mat4 rotation_matrix = glm::toMat4(glm::quat(rotation));
  glm::mat4 scale_matrix = glm::scale(scale);

  return translation_matrix * rotation_matrix * scale_matrix;
}

void Model::add_mesh(std::shared_ptr<Mesh> mesh)
{
  meshes_.push_back(std::move(mesh));
}

void Model::render(std::shared_ptr<ShaderProgram> shader)
{
  for (const auto& mesh : meshes_)
    mesh->render(shader);
}

void load_textures(
  std::shared_ptr<Mesh> mesh,
  aiMaterial* material,
  aiTextureType ai_texture_type,
  Texture::Type texture_type
) {
  for (unsigned int i = 0; i < material->GetTextureCount(ai_texture_type); i++)
  {
    aiString str;
    material->GetTexture(ai_texture_type, i, &str);
    mesh->add_texture(load_texture(str.C_Str(), texture_type));
  }
}

std::shared_ptr<Mesh> load_mesh(aiMesh* ai_mesh, const aiScene* ai_scene)
{
  std::vector<Vertex7> vertices;
  std::vector<uint32_t> indices;
  std::vector<std::shared_ptr<Texture>> textures;

  // vertices
  for (unsigned int i = 0; i < ai_mesh->mNumVertices; i++)
  {
    Vertex7 vertex;
    // position
    vertex.pos = glm::vec3(ai_mesh->mVertices[i].x, ai_mesh->mVertices[i].y, ai_mesh->mVertices[i].z);

    // normal
    if (ai_mesh->HasNormals())
      vertex.normal = glm::vec3(ai_mesh->mNormals[i].x, ai_mesh->mNormals[i].y, ai_mesh->mNormals[i].z);

    // texture coordinate
    if (ai_mesh->mTextureCoords[0])
    {
      vertex.uv = glm::vec2(ai_mesh->mTextureCoords[0][i].x, ai_mesh->mTextureCoords[0][i].y);
      vertex.tangent = glm::vec3(ai_mesh->mTangents[i].x, ai_mesh->mTangents[i].y, ai_mesh->mTangents[i].z);
      vertex.bitangent = glm::vec3(ai_mesh->mBitangents[i].x, ai_mesh->mBitangents[i].y, ai_mesh->mBitangents[i].z);
    }
    else
      vertex.uv = glm::vec2(0.0f, 0.0f);

    vertices.push_back(vertex);
  }

  // indices
  for (unsigned int i = 0; i < ai_mesh->mNumFaces; ++i)
    for (unsigned int j = 0; j < ai_mesh->mFaces[i].mNumIndices; ++j)
      indices.push_back(ai_mesh->mFaces[i].mIndices[j]);


  auto output_mesh = std::make_shared<Mesh>(vertices, indices);

  // materials
  aiMaterial* material = ai_scene->mMaterials[ai_mesh->mMaterialIndex];
  load_textures(output_mesh, material, aiTextureType_DIFFUSE, Texture::kDiffuse);
  load_textures(output_mesh, material, aiTextureType_SPECULAR, Texture::kSpecular);
  load_textures(output_mesh, material, aiTextureType_NORMALS, Texture::kNormal);
  load_textures(output_mesh, material, aiTextureType_HEIGHT, Texture::kHeight);

  return output_mesh;
}

void process_node(std::shared_ptr<Model> model, aiNode* ai_node, const aiScene* ai_scene)
{
  // add all mesh
  for (unsigned int i = 0; i < ai_node->mNumMeshes; ++i)
  {
    aiMesh* mesh = ai_scene->mMeshes[ai_node->mMeshes[i]];
    model->add_mesh(load_mesh(mesh, ai_scene));
  }

  // add children node mesh
  for (unsigned int i = 0; i < ai_node->mNumChildren; ++i)
    process_node(model, ai_node->mChildren[i], ai_scene);
}

std::shared_ptr<Model> Model::load_model(const std::string& path)
{
  Assimp::Importer importer;
  const aiScene* ai_scene = importer.ReadFile(path,
    aiProcess_Triangulate |
    aiProcess_GenSmoothNormals |
    aiProcess_FlipUVs |
    aiProcess_CalcTangentSpace);

  if (!ai_scene || ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode) // if is Not Zero
  {
    SPDLOG_ERROR("Failed to load model: {}", importer.GetErrorString());
    return nullptr;
  }

  auto model = std::make_shared<Model>();
  process_node(model, ai_scene->mRootNode, ai_scene);
  return model;
}

NAMESPACE_OPENGL_END
