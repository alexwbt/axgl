#pragma once

#include <string_view>

#include <axgl/interface/service.hpp>

namespace axgl::impl::opengl
{

class GuiService : public axgl::Service
{
public:
  static constexpr std::string_view kTypeId = "service::gui";
};

} // namespace axgl::impl::opengl
