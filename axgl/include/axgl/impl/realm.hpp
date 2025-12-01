#pragma once

#include <axgl/interface/realm.hpp>

#include <axgl/impl/entity_container.hpp>

namespace axgl::impl
{

class Realm : public axgl::Realm
{
  EntityContainer entities_;
  std::shared_ptr<Renderer> renderer_;

public:
  void tick(const Service::Context& context) override { entities_.tick({context, *this}); }

  void update(const Service::Context& context) override { entities_.update({context, *this}); }

  void render(const Service::Context& context) override
  {
    if (!renderer_ || !renderer_->ready())
      return;

    entities_.render({context, *this});
    renderer_->render();
  }

  void set_renderer(std::shared_ptr<Renderer> renderer) override { renderer_ = std::move(renderer); }
  [[nodiscard]] std::shared_ptr<Renderer> get_renderer() const override { return renderer_; }

  void add_entity(std::shared_ptr<Entity> entity) override { entities_.add(std::move(entity)); }
  void remove_entity(const std::shared_ptr<Entity> entity) override { entities_.remove(entity); }
  [[nodiscard]] util::Iterable<std::shared_ptr<Entity>> get_entities() const override { return entities_.get(); }
};

} // namespace axgl::impl
