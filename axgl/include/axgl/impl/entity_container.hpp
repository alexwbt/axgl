#pragma once

#include <memory>
#include <vector>

#include <axgl/interface/entity.hpp>

namespace axgl::impl
{

class EntityContainer
{
  std::vector<std::shared_ptr<Entity>> entities_;

public:
  void tick() const
  {
    for (const auto& entity : entities_)
      entity->tick();
  }

  void update()
  {
    for (const auto& entity : entities_)
    {
      if (entity->ticks() == 0)
        entity->on_create();

      entity->update();
    }

    if (!entities_.empty())
    {
      std::erase_if(entities_,
        [](const auto& entity)
        {
          const auto should_remove = entity->should_remove();
          if (should_remove)
            entity->on_remove();
          return should_remove;
        });
    }
  }

  void render() const
  {
    for (const auto& entity : entities_)
      entity->render();
  }

  void on_create() const
  {
    for (const auto& entity : entities_)
      entity->on_create();
  }

  void on_remove() const
  {
    for (const auto& entity : entities_)
      entity->on_remove();
  }

  void add_entity(std::shared_ptr<Entity> entity) { entities_.push_back(std::move(entity)); }

  void remove_entity(const std::shared_ptr<Entity>& entity) const
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

  util::Iterable<std::shared_ptr<Entity>> get_entities() const
  {
    return util::to_iterable_t<std::shared_ptr<Entity>>(entities_);
  }
};

} // namespace axgl::impl
