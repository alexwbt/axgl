#pragma once

#include <axgl/interface/realm.hpp>

#include <axgl/impl/entity_container.hpp>

namespace axgl::impl
{

class Realm : public axgl::Realm
{
  EntityContainer entities_;

  Context* context_ = nullptr;
  std::shared_ptr<Renderer> renderer_;

public:
  void set_renderer(std::shared_ptr<Renderer> renderer) override { renderer_ = std::move(renderer); }
  [[nodiscard]] std::shared_ptr<Renderer> get_renderer() const override { return renderer_; }

  void tick() override { entities_.tick(); }

  void update() override { entities_.update(); }

  void render() override
  {
    if (!renderer_ || !renderer_->ready())
      return;

    renderer_->before_render();

    entities_.render();

    renderer_->after_render();
  }

  void add_entity(std::shared_ptr<Entity> entity) override
  {
    entity->set_context(context_);
    entities_.add(std::move(entity));
  }

  void remove_entity(const std::shared_ptr<Entity> entity) override { entities_.remove(entity); }

  util::Iterable<std::shared_ptr<Entity>> get_entities() const override { return entities_.get(); }

  void set_context(Context* context) override
  {
    context_ = context;
    for (const auto& entity : get_entities())
      entity->set_context(context);
  }
};

} // namespace axgl::impl
