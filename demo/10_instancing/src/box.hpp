#pragma once

#include <cmath>

#include <axgl/impl/entity_base.hpp>

class Box final : public axgl::impl::EntityBase
{
public:
  static constexpr std::string_view kTypeId = "entity::box";

private:
  float y_ = 0.0f;
  float theta_ = 0.0f;
  float radius_ = 0.0f;
  float orbit_speed_ = 0.0f;
  glm::vec3 rotation_speed_{0.0f};

public:
  void on_create(const axgl::Realm::Context& context) override
  {
    const auto& position = transform().position;
    const glm::vec2 p2{position.x, position.z};
    y_ = position.y;
    theta_ = std::atan2(p2.y, p2.x);
    radius_ = glm::length(p2);
  }

  void update(const axgl::Realm::Context& context) override
  {
    auto& transform = this->transform();
    const auto delta_tick = static_cast<float>(context.delta_tick);
    theta_ += orbit_speed_ * delta_tick / radius_;
    transform.position.x = std::cos(theta_) * radius_;
    transform.position.z = std::sin(theta_) * radius_;
    transform.position.y = y_ + std::sin(theta_ * radius_);
    transform.rotation += rotation_speed_ * delta_tick;
    update_model_matrix();
  }

  void set_orbit_speed(float orbit_speed) { orbit_speed_ = orbit_speed; }
  void set_rotation_speed(glm::vec3 rotation_speed) { rotation_speed_ = rotation_speed; }
};
