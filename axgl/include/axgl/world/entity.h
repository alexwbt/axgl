#pragma once

#include <glm/glm.hpp>

#include "axgl/namespace.h"
#include "axgl/opengl/model.h"

NAMESPACE_WORLD

class Entity
{
  glm::vec3 position_;

  std::list<std::shared_ptr<axgl::opengl::Model>> models_;
  

public:
  virtual ~Entity() {}
  virtual void initialize() {}
  virtual void terminate() {}
  virtual void update() {}
  virtual void render() {}

  void add_model();
};

NAMESPACE_WORLD_END
