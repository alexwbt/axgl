#pragma once

#include <mutex>
#include <string>
#include <vector>

#include <imgui.h>

namespace ui
{

class ChatMessages
{
  std::vector<std::string> messages_;
  std::mutex mutex_;
  bool auto_scroll_ = true;

public:
  void add_message(const std::string& message)
  {
    std::lock_guard lock(mutex_);
    messages_.push_back(message);
    auto_scroll_ = true;
  }

  void clear_messages()
  {
    std::lock_guard lock(mutex_);
    messages_.clear();
    auto_scroll_ = true;
  }

  void render(const ImVec2& size)
  {
    std::lock_guard lock(mutex_);

    ImGui::BeginChild(
      "##chat_messages", size, ImGuiChildFlags_Border,
      ImGuiWindowFlags_HorizontalScrollbar
    );

    const bool scrolled_to_bottom
      = ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 8.0f;
    if (scrolled_to_bottom) auto_scroll_ = true;

    ImGui::PushTextWrapPos(ImGui::GetContentRegionAvail().x);
    for (const auto& message : messages_)
      ImGui::TextWrapped("%s", message.c_str());
    ImGui::PopTextWrapPos();

    if (auto_scroll_)
    {
      ImGui::SetScrollHereY(1.0f);
      auto_scroll_ = false;
    }

    ImGui::EndChild();
  }
};

} // namespace ui
