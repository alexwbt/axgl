#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/realm.hpp>

NAMESPACE_AXGL_IMPL

namespace component
{
  class Light : public interface::Component
  {
  public:
    interface::Light light;

    void update() override
    {
      if (disabled)
        return;

      auto parent = get_parent();
      auto context = get_context();

      light.position = parent->position;
      light.direction = parent->rotation;

      context->lights.push_back(&light);
    }
  };
}

NAMESPACE_AXGL_IMPL_END

NAMESPACE_AXGL

template<>
std::shared_ptr<impl::component::Light> interface::RealmService::create_component()
{
  return std::make_shared<impl::component::Light>();
}

NAMESPACE_AXGL_END
