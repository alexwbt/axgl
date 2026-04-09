#pragma once

#include <axgl/interface/context.hpp>

namespace axgl::impl
{

class ContextHolder : virtual public axgl::ContextHolder
{
protected:
  const axgl::Axgl* axgl_ = nullptr;
  const axgl::Context* context_ = nullptr;

  template <typename T, typename = std::enable_if_t<std::is_base_of_v<axgl::ContextHolder, T>>>
  axgl::ptr_t<T> with_context(axgl::ptr_t<T> holder)
  {
    holder->set_context(context_);
    return holder;
  }

public:
  void set_context(const axgl::Context* context) override
  {
    context_ = context;
    axgl_ = context->axgl;
  }
};

} // namespace axgl::impl
