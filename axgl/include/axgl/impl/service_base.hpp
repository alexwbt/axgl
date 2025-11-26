#pragma once

#ifdef AXGL_DEBUG
#include <stdexcept>
#endif

#include <axgl/interface/service.hpp>

namespace axgl::impl
{

class ServiceBase : virtual public Service
{
  const Context* context_ = nullptr;

public:
  void set_context(const Context* context) override
  {
#ifdef AXGL_DEBUG
    if (context_)
      throw std::runtime_error("Service context already set.");
#endif
    context_ = context;
  }

protected:
  [[nodiscard]] const Context* get_context() const override
  {
#ifdef AXGL_DEBUG
    if (!context_)
      throw std::runtime_error("ServiceContext is not provided here.");
#endif
    return context_;
  }
};

} // namespace axgl::impl

