#pragma once

#include <axgl/interface/realm.hpp>

#include <axgl/impl/entity_container.hpp>
#include <axgl/impl/services/sorted_render_service.hpp>

namespace axgl::impl
{

class Realm : public axgl::Realm
{
  EntityContainer entities_;
  std::shared_ptr<Renderer> renderer_;
  std::shared_ptr<SortedRenderService> sorted_render_service_;

public:
  void set_renderer(std::shared_ptr<Renderer> renderer) override { renderer_ = std::move(renderer); }
  [[nodiscard]] std::shared_ptr<Renderer> get_renderer() const override { return renderer_; }

  void tick(const Service::Context& context) override { entities_.tick({context, *this}); }

  void update(const Service::Context& context) override { entities_.update({context, *this}); }

  void render(const Service::Context& context) override
  {
    if (!renderer_ || !renderer_->ready())
      return;

    renderer_->before_render();
    entities_.render({context, *this});
    renderer_->after_render();
  }

  void add_entity(std::shared_ptr<Entity> entity) override { entities_.add(std::move(entity)); }
  void remove_entity(const std::shared_ptr<Entity> entity) override { entities_.remove(entity); }
  [[nodiscard]] util::Iterable<std::shared_ptr<Entity>> get_entities() const override { return entities_.get(); }
};

} // namespace axgl::impl
