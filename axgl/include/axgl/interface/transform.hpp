#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

namespace axgl
{

struct Transform final
{
  glm::vec3 scale{1.0f};
  glm::vec3 rotation{0.0f};
  glm::vec3 position{0.0f};

  glm::mat4 model_matrix{1.0f};

  void update_matrix()
  {
    model_matrix                                  //
      = glm::translate(glm::mat4(1.0f), position) //
      * glm::toMat4(glm::quat(rotation))          //
      * glm::scale(scale);
  }
};

} // namespace axgl
