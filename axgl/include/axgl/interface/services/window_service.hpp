#pragma once

#include <memory>

#include <axgl/interface/service.hpp>
#include <axgl/interface/window.hpp>

namespace axgl
{

class WindowService : virtual public Service
{
public:
  static constexpr std::string_view kTypeId = "service::window";

  virtual std::shared_ptr<Window> create_window() = 0;
};

} // namespace axgl
