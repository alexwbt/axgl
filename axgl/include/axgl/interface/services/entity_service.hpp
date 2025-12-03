#pragma once

#include <functional>
#include <string_view>

#include <axgl/common.hpp>
#include <axgl/interface/component.hpp>
#include <axgl/interface/entity.hpp>
#include <axgl/interface/service.hpp>

namespace axgl
{

class EntityService : virtual public axgl::Service
{
public:
  static constexpr std::string_view kTypeId = "service::entity";

  virtual void register_entity_factory(
    const std::string& type_id, std::function<axgl::ptr_t<axgl::Entity>()> entity_factory) = 0;
  virtual void register_component_factory(
    const std::string& type_id, std::function<axgl::ptr_t<axgl::Component>()> component_factory) = 0;

  virtual axgl::ptr_t<axgl::Entity> create_entity() = 0;
  virtual axgl::ptr_t<axgl::Entity> create_entity(const std::string& type_id) = 0;
  virtual axgl::ptr_t<axgl::Component> create_component(const std::string& type_id) = 0;

  template <typename EntityType>
  void register_entity_t()
  {
    register_entity_factory(EntityType::kTypeId.data(), [] { return axgl::create_ptr<EntityType>(); });
  }

  template <typename ComponentType>
  void register_component_t()
  {
    register_component_factory(ComponentType::kTypeId.data(), [] { return axgl::create_ptr<ComponentType>(); });
  }

  template <typename EntityType>
  axgl::ptr_t<EntityType> create_entity_t()
  {
    const auto type = EntityType::kTypeId.data();
    auto entity = axgl::ptr_cast<EntityType>(create_entity(type));
#ifdef AXGL_DEBUG
    if (!entity)
      throw std::runtime_error(std::format("Failed to create entity: {} ({})", type, typeid(EntityType).name()));
#endif
    return entity;
  }

  template <typename ComponentType>
  axgl::ptr_t<ComponentType> create_component_t()
  {
    const auto type = ComponentType::kTypeId.data();
    auto component = axgl::ptr_cast<ComponentType>(create_component(type));
#ifdef AXGL_DEBUG
    if (!component)
      throw std::runtime_error(std::format("Failed to create component: {} ({})", type, typeid(ComponentType).name()));
#endif
    return component;
  }
};

} // namespace axgl
