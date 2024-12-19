#pragma once

#include <axgl/except.hpp>
#include <axgl/namespace.hpp>
#include <axgl/interface/gui.hpp>

NAMESPACE_AXGL_IMPL

class OpenglGuiService : public interface::GuiService
{
private:
  std::vector<std::shared_ptr<interface::GuiElement>> pages_;

public:
  void add_page(std::shared_ptr<interface::GuiElement> page) override
  {
    pages_.push_back(std::move(page));
  }
};

NAMESPACE_AXGL_IMPL_END

NAMESPACE_AXGL

template<>
std::shared_ptr<impl::OpenglGuiService> Axgl::use_service()
{
  // create opengl service
  auto gui_service = std::make_shared<impl::OpenglGuiService>();
  register_service("gui", gui_service);

  return gui_service;
}

NAMESPACE_AXGL_END
