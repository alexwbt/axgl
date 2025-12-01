#pragma once

#include <axgl/interface/realm.hpp>

#include <axgl/impl/entity_container.hpp>

namespace axgl::impl
{

class Realm : public axgl::Realm
{
  EntityContainer entities_;
  ptr_t<Renderer> renderer_;

public:
  void tick(const Service::Context& context) override { entities_.tick({context, *this, renderer_.get()}); }

  void update(const Service::Context& context) override { entities_.update({context, *this, renderer_.get()}); }

  void render(const Service::Context& context) override
  {
    if (!renderer_ || !renderer_->ready())
      return;

    entities_.render({context, *this, renderer_.get()});
    renderer_->render();
  }

  void set_renderer(ptr_t<Renderer> renderer) override { renderer_ = std::move(renderer); }
  [[nodiscard]] ptr_t<Renderer> get_renderer() const override { return renderer_; }

  void add_entity(ptr_t<Entity> entity) override { entities_.add(std::move(entity)); }
  void remove_entity(const ptr_t<Entity> entity) override { entities_.remove(entity); }
  [[nodiscard]] util::Iterable<ptr_t<Entity>> get_entities() const override { return entities_.get(); }
};

} // namespace axgl::impl
