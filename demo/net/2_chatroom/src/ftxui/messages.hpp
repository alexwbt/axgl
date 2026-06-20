#pragma once

#include <ftxui/component/component_base.hpp>

namespace tui
{

class Messages : public ftxui::ComponentBase
{
public:
  [[nodiscard]] bool Focusable() const override { return true; }

  ftxui::Element OnRender() override { }
};

} // namespace tui
