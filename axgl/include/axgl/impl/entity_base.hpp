#pragma once

#ifdef AXGL_DEBUG
#include <stdexcept>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

#include <axgl/interface/entity.hpp>
#include <axgl/interface/transform.hpp>

#include <axgl/impl/component_container.hpp>

namespace axgl::impl
{

class EntityBase : virtual public Entity
{
  std::string id_;
  Realm::Context* context_ = nullptr;

  bool disabled_ = false;
  std::uint32_t ticks_ = 0;

  glm::mat4 model_matrix_{1.0f};
  Transform transform_;

  bool should_remove_ = false;

protected:
  ComponentContainer components_{this};

public:
  void tick() override
  {
    ++ticks_;
    components_.tick();
  }
  void update() override { components_.update(); }
  void render() override { components_.render(); }
  void on_create() override { components_.on_create(); }
  void on_remove() override { components_.on_remove(); }

  uint32_t ticks() const override { return ticks_; }
  void set_disabled(const bool disabled) override { disabled_ = disabled; }
  bool is_disabled() const override { return disabled_; }

  void set_id(const std::string& id) override { id_ = id; }
  std::string get_id() const override { return id_; }

  Transform* transform() override { return &transform_; }
  void update_model_matrix() override
  {
    model_matrix_ = glm::translate(glm::mat4(1.0f), transform_.position) * glm::toMat4(glm::quat(transform_.rotation)) *
                    glm::scale(transform_.scale);
  }
  glm::mat4 get_model_matrix() const override { return model_matrix_; }

  void mark_remove(const bool should_remove) override { should_remove_ = should_remove; }
  bool should_remove() const override { return should_remove_; }

  ComponentManager* components() override { return &components_; }

  void set_context(Realm::Context* context) override { context_ = context; }
  Realm::Context* get_context() override
  {
#ifdef AXGL_DEBUG
    if (!context_)
      throw std::runtime_error("Entity context is not set.");
#endif
    return context_;
  }
};

} // namespace axgl::impl
