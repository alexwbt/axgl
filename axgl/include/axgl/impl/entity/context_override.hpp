#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/realm.hpp>
#include <axgl/impl/realm_service.hpp>

namespace axgl::impl::entity
{

class ContextOverride : public Entity
{
protected:
  interface::RealmContext* parent_context_ = nullptr;

  interface::Camera camera_;
  interface::RealmContext context_{nullptr, nullptr, &camera_, {}};

public:
  void update() override
  {
    if (parent_context_)
    {
      context_.axgl = parent_context_->axgl;
      context_.realm = parent_context_->realm;
    }
    Entity::update();
  }

  void set_context(interface::RealmContext* context) override
  {
    parent_context_ = context;
    Entity::set_context(&context_);
  }
};

} // namespace axgl::impl::entity

namespace axgl
{

template <> inline std::shared_ptr<impl::entity::ContextOverride> interface::RealmService::create_entity()
{
  return std::make_shared<impl::entity::ContextOverride>();
}

} // namespace axgl
