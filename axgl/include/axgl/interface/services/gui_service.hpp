#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/service.hpp>

namespace axgl
{

class GuiService : virtual public axgl::Service
{
public:
  static constexpr std::string_view kTypeId = "service::gui";

  virtual void rerender() = 0;
};

} // namespace axgl
