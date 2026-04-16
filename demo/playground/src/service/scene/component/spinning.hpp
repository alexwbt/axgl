#pragma once

#include <axgl/impl/component_base.hpp>

class SpinningComponent : public axgl::impl::ComponentBase
{
public:
  static constexpr std::string_view kTypeId = "component::spinning";

private:
  glm::vec3 rotation_speed_{0.0f};

public:
  void parent_update(const axgl::Entity* parent) override
  {
    ComponentBase::parent_update(parent);
    parent->transform().rotation += rotation_speed_ * context_->delta_tick_f;
    parent->transform().update_matrix();
  }

  void set_rotation_speed(glm::vec3 rotation_speed) { rotation_speed_ = rotation_speed; }
};
