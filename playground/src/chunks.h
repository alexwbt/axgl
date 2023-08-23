#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "axgl/world/entity.h"
#include "axgl/opengl/model.h"
#include "axgl/opengl/shader/phong-shader.h"

class Chunk : public axgl::world::Entity
{
public:
  static constexpr int kSize = 32;

private:
  float data_[kSize][kSize][kSize]{};
  std::shared_ptr<axgl::opengl::Model> model_;
  std::shared_ptr<axgl::opengl::shader::PhongShader> shader_;

public:
  Chunk();
  void generate_model();
  void render(const axgl::world::RenderContext& context);

private:
  bool in_bound(const glm::vec3& pos);

  static float noise(const glm::vec3& block_pos);
  static void generate_data(float(*data)[kSize][kSize]);
};
