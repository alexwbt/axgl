#pragma once

#include <axgl/impl/component_base.hpp>

class SpinningComponent : public axgl::impl::ComponentBase
{
public:
  static constexpr std::string_view kTypeId = "component::spinning";

private:
  glm::vec3 rotation_speed_{0.0f};

public:
  void update(const axgl::Entity::Context& context) override
  {
    ComponentBase::update(context);

    context.entity.transform().rotation += rotation_speed_ * context.delta_tick_f;
    context.entity.update_model_matrix();
  }

  void set_rotation_speed(glm::vec3 rotation_speed) { rotation_speed_ = rotation_speed; }
};
