#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/realm.hpp>

#include <axgl/impl/entity_container.hpp>

namespace axgl::impl
{

class Realm : public axgl::Realm
{
  EntityContainer entities_;
  axgl::ptr_t<Renderer> renderer_;

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

  void set_renderer(axgl::ptr_t<Renderer> renderer) override { renderer_ = std::move(renderer); }
  [[nodiscard]] axgl::ptr_t<Renderer> get_renderer() const override { return renderer_; }

  [[nodiscard]] EntityContainer& entities() override { return entities_; }
};

} // namespace axgl::impl
