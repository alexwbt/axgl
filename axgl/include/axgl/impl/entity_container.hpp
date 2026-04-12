#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/container.hpp>
#include <axgl/interface/entity.hpp>

#include <axgl/impl/context_holder.hpp>

namespace axgl::impl
{

class EntityContainer : virtual public axgl::Container<axgl::Entity>, public axgl::impl::ContextHolder
{
  axgl::Entity* const parent_;
  std::vector<axgl::ptr_t<axgl::Entity>> new_entities_;
  std::vector<axgl::ptr_t<axgl::Entity>> entities_;

public:
  explicit EntityContainer(axgl::Entity* parent) : parent_(parent) { }

  void set_context(const axgl::Context* context) override
  {
    axgl::impl::ContextHolder::set_context(context);

    for (const auto& entity : entities_)
      entity->set_context(context);

    for (const auto& entity : new_entities_)
      entity->set_context(context);
  }

  void tick() const
  {
    for (const auto& entity : entities_)
      if (!entity->is_disabled() && !entity->is_static()) entity->parent_tick(parent_);
  }

  void update()
  {
    if (!new_entities_.empty())
    {
      entities_.insert(entities_.end(), new_entities_.begin(), new_entities_.end());
      new_entities_.clear();
    }

    if (!entities_.empty())
    {
      for (const auto& entity : entities_)
        if (!entity->is_disabled() && !entity->is_static())
        {
          if (entity->ticks() == 0) entity->on_parent_create(parent_);
          entity->parent_update(parent_);
        }

      std::erase_if(
        entities_, [&](const auto& entity)
      {
        const auto should_remove = entity->should_remove();
        if (should_remove) entity->on_remove();
        return should_remove;
      });
    }
  }

  void on_create() const
  {
    for (const auto& entity : entities_)
      if (!entity->is_disabled() && !entity->is_static()) entity->on_parent_create(parent_);
  }

  void on_remove() const
  {
    for (const auto& entity : entities_)
      if (!entity->is_disabled() && !entity->is_static()) entity->on_parent_remove(parent_);
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
    entity->set_context(context_);
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
  [[nodiscard]] std::uint64_t size() const override { return entities_.size(); }
  [[nodiscard]] bool empty() const override { return entities_.empty(); }
};

} // namespace axgl::impl
