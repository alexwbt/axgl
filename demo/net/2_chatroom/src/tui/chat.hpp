#pragma once

#include <memory>

#include "chat_input.hpp"
#include "chat_messages.hpp"

namespace tui
{

class Chat : public ftxui::ComponentBase
{
  std::shared_ptr<ChatInput> input_comp_;
  std::shared_ptr<ChatMessages> messages_comp_;

public:
  explicit Chat()
  {
    messages_comp_ = std::make_shared<ChatMessages>();
    input_comp_ = std::make_shared<ChatInput>([this](const auto& message) { on_message(message); });

    Add(input_comp_);
    Add(messages_comp_);
  }

  void on_message(const std::string& message)
  {
    if (message.empty()) return;

    messages_comp_->add_message(message);
  }

  ftxui::Element OnRender() override
  {
    return ftxui::vbox({
      messages_comp_->Render() | ftxui::vscroll_indicator | ftxui::yframe | ftxui::flex,
      input_comp_->Render(),
    });
  }
};

} // namespace tui
