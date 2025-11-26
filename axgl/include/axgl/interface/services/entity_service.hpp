#pragma once

#include <functional>
#include <memory>

#include <axgl/interface/component.hpp>
#include <axgl/interface/entity.hpp>
#include <axgl/interface/service.hpp>

namespace axgl
{

class EntityService : virtual public Service
{
public:
  virtual void register_entity_factory(
    const std::string& type, std::function<std::shared_ptr<Entity>()> entity_factory) = 0;
  virtual void register_component_factory(
    const std::string& type, std::function<std::shared_ptr<Component>()> component_factory) = 0;

  virtual std::shared_ptr<Entity> create_entity() = 0;
  virtual std::shared_ptr<Entity> create_entity(const std::string& type) = 0;
  virtual std::shared_ptr<Component> create_component(const std::string& type) = 0;

  template <typename EntityType>
  void register_entity_t()
  {
    register_entity_factory(EntityType::kType.data(),
      []
      {
        return std::make_shared<EntityType>();
      });
  }

  template <typename ComponentType>
  void register_component_t()
  {
    register_component_factory(ComponentType::kType.data(),
      []
      {
        return std::make_shared<ComponentType>();
      });
  }

  template <typename EntityType>
  std::shared_ptr<EntityType> create_entity_t()
  {
    const auto type = EntityType::kType.data();
    auto entity = std::dynamic_pointer_cast<EntityType>(create_entity(type));
#ifdef AXGL_DEBUG
    if (!entity)
      throw std::runtime_error(std::format("Failed to create entity: {} ({})", type, typeid(EntityType).name()));
#endif
    return entity;
  }

  template <typename ComponentType>
  std::shared_ptr<ComponentType> create_component_t()
  {
    const auto type = ComponentType::kType.data();
    auto component = std::dynamic_pointer_cast<ComponentType>(create_component(type));
#ifdef AXGL_DEBUG
    if (!component)
      throw std::runtime_error(std::format("Failed to create component: {} ({})", type, typeid(ComponentType).name()));
#endif
    return component;
  }
};

} // namespace axgl
