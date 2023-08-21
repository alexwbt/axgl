#include "axgl/opengl/model.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

NAMESPACE_OPENGL

Model::~Model()
{
  if (vao_id_ > 0) glDeleteVertexArrays(1, &vao_id_);
  if (vbo_id_ > 0) glDeleteBuffers(1, &vbo_id_);
  if (ebo_id_ > 0) glDeleteBuffers(1, &ebo_id_);
}

glm::mat4 Model::model_matrix()
{
    glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), translation);
    glm::mat4 rotation_matrix = glm::toMat4(glm::quat(rotation));
    glm::mat4 scale_matrix = glm::scale(scale);

    return translation_matrix * rotation_matrix * scale_matrix;
}

void Model::render(const ShaderProgram::Uniforms& uniforms)
{
  shader_->use(uniforms);

  if (textures_)
    for (int i = 0; i < textures_->size(); ++i)
      textures_->at(i)->use(i);

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

NAMESPACE_OPENGL_END
