#pragma once

#include <axgl/common.hpp>
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
