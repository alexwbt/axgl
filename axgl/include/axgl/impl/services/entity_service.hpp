#pragma once

#include <format>
#include <functional>
#include <unordered_map>

#include <axgl/common.hpp>
#include <axgl/interface/services/entity_service.hpp>

#include <axgl/impl/entity_base.hpp>

namespace axgl::impl
{

class EntityService : virtual public axgl::EntityService
{
  std::unordered_map<std::string, std::function<axgl::ptr_t<axgl::Entity>()>> entity_factories_;
  std::unordered_map<std::string, std::function<axgl::ptr_t<axgl::Component>()>> component_factories_;

public:
  void register_entity_factory(const std::string& type, std::function<ptr_t<axgl::Entity>()> entity_factory) override
  {
    entity_factories_.insert({type, entity_factory});
  }

  void register_component_factory(const std::string& type, std::function<ptr_t<Component>()> component_factory) override
  {
    component_factories_.insert({type, component_factory});
  }

  ptr_t<axgl::Entity> create_entity() override { return axgl::create_ptr<impl::EntityBase>(); }

  ptr_t<axgl::Entity> create_entity(const std::string& type) override
  {
#ifdef AXGL_DEBUG
    if (!entity_factories_.contains(type))
      throw std::runtime_error(std::format("Entity factory for '{}' not registered.", type));
#endif
    return entity_factories_.at(type)();
  }

  ptr_t<Component> create_component(const std::string& type) override
  {
#ifdef AXGL_DEBUG
    if (!component_factories_.contains(type))
      throw std::runtime_error(std::format("Component factory for '{}' not registered.", type));
#endif
    return component_factories_.at(type)();
  }
};

} // namespace axgl::impl
