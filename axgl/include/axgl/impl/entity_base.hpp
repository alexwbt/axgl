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
  std::uint64_t ticks_ = 0;
  bool disabled_ = false;
  bool should_remove_ = false;

  Transform transform_;
  glm::mat4 model_matrix_{1.0f};

protected:
  ComponentContainer components_;

public:
  void tick(const Realm::Context& context) override
  {
    ++ticks_;
    components_.tick({context, *this});
  }
  void update(const Realm::Context& context) override { components_.update({context, *this}); }
  void render(const Realm::Context& context) override { components_.render({context, *this}); }
  void on_create(const Realm::Context& context) override { components_.on_entity_create({context, *this}); }
  void on_remove(const Realm::Context& context) override { components_.on_entity_remove({context, *this}); }

  [[nodiscard]] std::uint64_t ticks() const override { return ticks_; }

  void set_disabled(const bool disabled) override { disabled_ = disabled; }
  [[nodiscard]] bool is_disabled() const override { return disabled_; }

  void set_id(const std::string& id) override { id_ = id; }
  [[nodiscard]] std::string get_id() const override { return id_; }

  Transform& transform() override { return transform_; }
  void update_model_matrix() override
  {
    model_matrix_ = glm::translate(glm::mat4(1.0f), transform_.position) * glm::toMat4(glm::quat(transform_.rotation)) *
                    glm::scale(transform_.scale);
  }
  [[nodiscard]] glm::mat4 get_model_matrix() const override { return model_matrix_; }

  void mark_remove(const bool should_remove) override { should_remove_ = should_remove; }
  [[nodiscard]] bool should_remove() const override { return should_remove_; }

  ComponentContainer* components() override { return &components_; }
};

} // namespace axgl::impl
