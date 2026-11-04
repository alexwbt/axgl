#pragma once

#include <glm/glm.hpp>

#include <axgl/common.hpp>
#include <axgl/interface/context.hpp>

namespace axgl {

class PhysicsShape : public virtual axgl::ContextHolder {
public:
  virtual ~PhysicsShape() = default;
};

} // namespace axgl
