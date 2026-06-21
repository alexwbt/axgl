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
  explicit Chat(const std::function<void(const std::string&)>& on_input) :
    input_comp_(std::make_shared<ChatInput>(on_input)), messages_comp_(std::make_shared<ChatMessages>())
  {
    Add(input_comp_);
    Add(messages_comp_);
  }

  void add_message(const std::string& message) const
  {
    if (message.empty()) return;

    messages_comp_->add_message(message);
  }

  void clear_messages() const { messages_comp_->clear_messages(); }

  ftxui::Element OnRender() override
  {
    return ftxui::vbox({
      messages_comp_->Render() | ftxui::vscroll_indicator | ftxui::yframe | ftxui::flex,
      input_comp_->Render(),
    });
  }
};

} // namespace tui
