#pragma once

#include <functional>
#include <string>

#include <imgui.h>

#include "chat_input.hpp"
#include "chat_messages.hpp"

namespace ui
{

class Chat
{
  ChatMessages messages_;
  ChatInput input_;

public:
  explicit Chat(const std::function<void(const std::string&)>& on_input) :
    input_(on_input)
  {
  }

  void add_message(const std::string& message)
  {
    messages_.add_message(message);
  }

  void clear_messages() { messages_.clear_messages(); }

  void render(bool connected)
  {
    input_.set_enabled(connected);
    messages_.render(ImVec2(
      0,
      -ImGui::GetFrameHeightWithSpacing() - ImGui::GetStyle().ItemSpacing.y));
    ImGui::Spacing();
    input_.render();
  }
};

} // namespace ui
