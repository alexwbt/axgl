#pragma once

#include <vector>

#include <axgl/common.hpp>
#include <axgl/interface/container.hpp>
#include <axgl/interface/entity.hpp>

namespace axgl::impl
{

class EntityContainer : public virtual axgl::Container<axgl::Entity>
{
  std::vector<axgl::ptr_t<axgl::Entity>> entities_;

public:
  void tick(const axgl::Realm::Context& context) const
  {
    for (const auto& entity : entities_)
      entity->tick(context);
  }

  void update(const axgl::Realm::Context& context)
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

  void on_create(const axgl::Realm::Context& context) const
  {
    for (const auto& entity : entities_)
      entity->on_create(context);
  }

  void on_remove(const axgl::Realm::Context& context) const
  {
    for (const auto& entity : entities_)
      entity->on_remove(context);
  }

  void add(axgl::ptr_t<axgl::Entity> entity) override { entities_.push_back(std::move(entity)); }
  void remove(const axgl::ptr_t<axgl::Entity>& entity) override
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
  [[nodiscard]] util::Iterable<axgl::ptr_t<axgl::Entity>> get() const override
  {
    return util::to_iterable_t<axgl::ptr_t<axgl::Entity>>(entities_);
  }
};

} // namespace axgl::impl
