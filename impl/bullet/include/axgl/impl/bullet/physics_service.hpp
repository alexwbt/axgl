#pragma once

#include <format>
#include <functional>
#include <memory>
#include <unordered_map>

#include <btBulletDynamicsCommon.h>

#include <axgl/common.hpp>
#include <axgl/interface/services/physics_service.hpp>

#include <axgl/impl/bullet/physics_shape.hpp>
#include <axgl/impl/service_base.hpp>

namespace axgl::impl::bullet {

class PhysicsService : public virtual axgl::PhysicsService,
                       public axgl::impl::ServiceBase {
public:
  static constexpr std::string_view kTypeId = "service::physics";

private:
  using ShapeFactory = std::function<axgl::ptr_t<axgl::PhysicsShape>()>;
  std::unordered_map<std::string, ShapeFactory> shape_factories_;

  std::unique_ptr<btDefaultCollisionConfiguration> collision_configuration_;
  std::unique_ptr<btCollisionDispatcher> dispatcher_;
  std::unique_ptr<btBroadphaseInterface> broadphase_;
  std::unique_ptr<btSequentialImpulseConstraintSolver> solver_;
  std::unique_ptr<btDiscreteDynamicsWorld> world_;

  glm::vec3 gravity_{0.0f, -9.8f, 0.0f};

public:
  void initialize() override {
    collision_configuration_
      = std::make_unique<btDefaultCollisionConfiguration>();
    dispatcher_
      = std::make_unique<btCollisionDispatcher>(collision_configuration_.get());
    broadphase_ = std::make_unique<btDbvtBroadphase>();
    solver_ = std::make_unique<btSequentialImpulseConstraintSolver>();
    world_ = std::make_unique<btDiscreteDynamicsWorld>(
      dispatcher_.get(),
      broadphase_.get(),
      solver_.get(),
      collision_configuration_.get()
    );
    world_->setGravity(btVector3(gravity_.x, gravity_.y, gravity_.z));
  }

  void terminate() override {
    world_.reset();
    solver_.reset();
    broadphase_.reset();
    dispatcher_.reset();
    collision_configuration_.reset();
  }

  void set_gravity(const glm::vec3& gravity) override {
    gravity_ = gravity;
    if (world_) world_->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
  }

  [[nodiscard]] glm::vec3 get_gravity() const override { return gravity_; }

  void step_simulation(float time_step) override {
    if (world_) world_->stepSimulation(time_step);
  }

  void register_shape_factory(
    const std::string& shape_id,
    std::function<axgl::ptr_t<axgl::PhysicsShape>()> shape_factory
  ) override {
    shape_factories_.emplace(shape_id, std::move(shape_factory));
  }

  axgl::ptr_t<axgl::PhysicsShape> create_shape(
    const std::string& shape_id
  ) override {
#ifdef AXGL_DEBUG
    if (shape_factories_.count(shape_id) == 0)
      throw std::runtime_error(
        std::format("Physics shape factory for '{}' not registered.", shape_id)
      );
#endif
    return with_context(shape_factories_.at(shape_id)());
  }

  [[nodiscard]] std::int32_t priority() const override { return 400; }

  btDiscreteDynamicsWorld* world() { return world_.get(); }
};

} // namespace axgl::impl::bullet
