#pragma once

#include <axgl/interface/context.hpp>

namespace axgl::impl
{

class ContextHolder : virtual public axgl::ContextHolder
{
protected:
  const axgl::Context* context_ = nullptr;

public:
  void set_context(const Context* context) override { context_ = context; }
};

} // namespace axgl::impl
