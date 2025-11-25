#pragma once

#include <format>
#include <functional>
#include <unordered_map>

#include <axgl/interface/services/entity_service.hpp>

#include <axgl/impl/entity_base.hpp>
#include <axgl/impl/service_base.hpp>

namespace axgl::impl
{

class EntityService : virtual public axgl::EntityService, public ServiceBase
{
  std::unordered_map<std::string, std::function<std::shared_ptr<Entity>()>> entity_factories_;
  std::unordered_map<std::string, std::function<std::shared_ptr<Component>()>> component_factories_;

public:
  void register_entity_factory(
    const std::string& type, std::function<std::shared_ptr<Entity>()> entity_factory) override
  {
    entity_factories_.insert({type, entity_factory});
  }

  void register_component_factory(
    const std::string& type, std::function<std::shared_ptr<Component>()> component_factory) override
  {
    component_factories_.insert({type, component_factory});
  }

  std::shared_ptr<Entity> create_entity() override { return std::make_shared<impl::EntityBase>(); }

  std::shared_ptr<Entity> create_entity(const std::string& type) override
  {
#ifdef AXGL_DEBUG
    if (!entity_factories_.contains(type))
      throw std::runtime_error(std::format("Entity factory for '{}' not found.", type));
#endif
    return entity_factories_.at(type)();
  }

  std::shared_ptr<Component> create_component(const std::string& type) override
  {
#ifdef AXGL_DEBUG
    if (!component_factories_.contains(type))
      throw std::runtime_error(std::format("Component factory for '{}' not found.", type));
#endif
    return component_factories_.at(type)();
  }
};

} // namespace axgl::impl
