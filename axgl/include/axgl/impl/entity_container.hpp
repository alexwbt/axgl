#pragma once

#include <memory>
#include <vector>

#include <axgl/interface/container.hpp>
#include <axgl/interface/entity.hpp>

namespace axgl::impl
{

class EntityContainer : public virtual Container<axgl::Entity>
{
  std::vector<std::shared_ptr<axgl::Entity>> entities_;

public:
  void tick(const Realm::Context& context) const
  {
    for (const auto& entity : entities_)
      entity->tick(context);
  }

  void update(const Realm::Context& context)
  {
    for (const auto& entity : entities_)
    {
      if (entity->ticks() == 0)
        entity->on_create(context);

      entity->update(context);
    }

    if (!entities_.empty())
    {
      std::erase_if(
        entities_, [&context](const auto& entity)
      {
        const auto should_remove = entity->should_remove();
        if (should_remove)
          entity->on_remove(context);
        return should_remove;
      });
    }
  }

  void render(const Realm::Context& context) const
  {
    for (const auto& entity : entities_)
      entity->render(context);
  }

  void on_create(const Realm::Context& context) const
  {
    for (const auto& entity : entities_)
      entity->on_create(context);
  }

  void on_remove(const Realm::Context& context) const
  {
    for (const auto& entity : entities_)
      entity->on_remove(context);
  }

  void add(std::shared_ptr<axgl::Entity> entity) override { entities_.push_back(std::move(entity)); }
  void remove(const std::shared_ptr<axgl::Entity>& entity) override
  {
    for (auto& e : entities_)
    {
      if (e == entity)
      {
        e->mark_remove(true);
        break;
      }
    }
  }
  [[nodiscard]] util::Iterable<std::shared_ptr<axgl::Entity>> get() const override
  {
    return util::to_iterable_t<std::shared_ptr<axgl::Entity>>(entities_);
  }
};

} // namespace axgl::impl
