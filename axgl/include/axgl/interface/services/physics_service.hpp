#pragma once

#include <functional>

#include <glm/glm.hpp>

#include <axgl/common.hpp>
#include <axgl/interface/components/rigidbody.hpp>
#include <axgl/interface/physics_shape.hpp>
#include <axgl/interface/service.hpp>

namespace axgl {

class PhysicsService : public virtual axgl::Service {
public:
  static constexpr std::string_view kTypeId = "service::physics";

  virtual void set_gravity(const glm::vec3& gravity) = 0;
  [[nodiscard]] virtual glm::vec3 get_gravity() const = 0;
  virtual void step_simulation(float time_step) = 0;

  virtual void register_shape_factory(
    const std::string& shape_id,
    std::function<axgl::ptr_t<axgl::PhysicsShape>()> shape_factory
  ) = 0;

  template <typename ShapeType>
  void register_shape_t() {
    register_shape_factory(ShapeType::kTypeId.data(), [] {
      return axgl::create_ptr<ShapeType>();
    });
  }

  template <typename ShapeType>
  axgl::ptr_t<ShapeType> create_shape_t() {
    const auto type = ShapeType::kTypeId.data();
    auto shape = axgl::ptr_cast<ShapeType>(create_shape(type));
    return shape;
  }

  virtual axgl::ptr_t<axgl::PhysicsShape> create_shape(
    const std::string& shape_id
  ) = 0;
};

} // namespace axgl
