#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/entity_base.hpp>

class Cube : public axgl::impl::EntityBase
{
  bool rotate_ = false;
  glm::vec3 rotation_speed_{0.0f};

public:
  void update(const axgl::Realm::Context& context) override
  {
    EntityBase::update(context);

    if (rotate_)
    {
      auto& transform = this->transform();
      const auto delta_tick = static_cast<float>(context.delta_tick);

      transform.rotation += rotation_speed_ * delta_tick;
      update_model_matrix();
    }
  }

  void set_rotation_speed(glm::vec3 rotation_speed)
  {
    rotation_speed_ = rotation_speed;
    rotate_ = true;
  }
};
