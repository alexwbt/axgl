#pragma once

#include <axgl/interface/service.hpp>

#include <axgl/impl/context_holder.hpp>

namespace axgl::impl
{

class ServiceBase : virtual public axgl::Service, public axgl::impl::ContextHolder
{
};

} // namespace axgl::impl
