#pragma once

#include <axgl/impl/entity_base.hpp>

class Box final : public axgl::impl::EntityBase
{
public:
  static constexpr std::string_view kTypeId = "entity::box";

private:
  glm::vec3 rotation_speed_{0.0f};

public:
  void set_rotation_speed(glm::vec3 rotation_speed) { rotation_speed_ = rotation_speed; }

  void update(const axgl::Realm::Context& context) override
  {
    transform().rotation += rotation_speed_ * static_cast<float>(context.delta_tick);
    update_model_matrix();
  }
};
