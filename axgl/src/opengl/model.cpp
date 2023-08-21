#include "axgl/opengl/model.h"

NAMESPACE_OPENGL

Model::~Model()
{
  if (vao_id_ > 0) glDeleteVertexArrays(1, &vao_id_);
  if (vbo_id_ > 0) glDeleteBuffers(1, &vbo_id_);
  if (ebo_id_ > 0) glDeleteBuffers(1, &ebo_id_);
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
