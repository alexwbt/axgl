#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/container.hpp>
#include <axgl/interface/entity.hpp>

namespace axgl::impl
{

class EntityContainer : public virtual axgl::Container<axgl::Entity>
{
  std::vector<axgl::ptr_t<axgl::Entity>> new_entities_;
  std::vector<axgl::ptr_t<axgl::Entity>> entities_;

public:
  void tick(const axgl::Realm::Context& context) const
  {
    for (const auto& entity : entities_)
      entity->tick(context);
  }

  void update(const axgl::Realm::Context& context)
  {
    entities_.insert(entities_.end(), new_entities_.begin(), new_entities_.end());
    new_entities_.clear();

    for (const auto& entity : entities_)
    {
      if (entity->ticks() == 0) entity->on_create(context);

      entity->update(context);
    }

    if (!entities_.empty())
    {
      std::erase_if(
        entities_, [&context](const auto& entity)
      {
        const auto should_remove = entity->should_remove();
        if (should_remove) entity->on_remove(context);
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

  [[nodiscard]] auto get_by_id(std::uint64_t id) const
  {
    const auto it = std::ranges::find_if(entities_, [id](const auto& e) { return e->get_id() == id; });
    return it != entities_.end() ? *it : nullptr;
  }

  void add(axgl::ptr_t<axgl::Entity> entity) override
  {
#ifdef AXGL_DEBUG
    if (
      std::ranges::any_of(entities_, [&entity](const auto& e) { return e == entity; })
      || std::ranges::any_of(new_entities_, [&entity](const auto& e) { return e == entity; }))
    {
      AXGL_LOG_DEBUG("Entity(id: {}, name: {}) already exists.", entity->get_id(), entity->get_name());
      return;
    }
#endif
    new_entities_.push_back(std::move(entity));
  }

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
    std::erase_if(new_entities_, [&entity](const auto& e) { return e == entity; });
  }
  [[nodiscard]] std::span<const axgl::ptr_t<axgl::Entity>> get() const override { return entities_; }
};

} // namespace axgl::impl
