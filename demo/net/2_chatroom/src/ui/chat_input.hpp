#pragma once

#include <functional>
#include <string>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace ui
{

class ChatInput
{
  std::string input_value_;
  std::function<void(const std::string&)> on_enter_;
  bool enabled_ = true;

public:
  explicit ChatInput(std::function<void(const std::string&)> on_enter) :
    on_enter_(std::move(on_enter))
  {
  }

  void set_enabled(bool enabled) { enabled_ = enabled; }

  void render()
  {
    if (!enabled_) ImGui::BeginDisabled();

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 64.0f);
    if (ImGui::InputText(
          "##chat_input", &input_value_, ImGuiInputTextFlags_EnterReturnsTrue))
    {
      submit();
    }
    ImGui::SameLine();
    if (ImGui::Button("Send", ImVec2(64.0f, 0))) { submit(); }

    if (!enabled_) ImGui::EndDisabled();
  }

private:
  void submit()
  {
    const auto message = input_value_;
    input_value_.clear();
    if (!message.empty()) on_enter_(message);
  }
};

} // namespace ui
