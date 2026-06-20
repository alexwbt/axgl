#pragma once

#include <functional>

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>

namespace tui
{

class ChatInput : public ftxui::ComponentBase
{
  std::string input_value_;
  std::function<void(const std::string&)> on_enter_;

public:
  explicit ChatInput(const std::function<void(const std::string&)>& on_enter) : on_enter_(on_enter)
  {
    Add(ftxui::Input(&input_value_));
  }

  bool OnEvent(ftxui::Event event) override
  {
    if (event == ftxui::Event::Return)
    {
      on_enter_(input_value_);
      input_value_.clear();
      return true;
    }

    return ftxui::ComponentBase::OnEvent(event);
  }
};

} // namespace tui
