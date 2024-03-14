#pragma once

#include "event.h"

class Console : public axgl::Component
{
  std::string input_;
  std::string history_;

public:
  void on_enter(axgl::ComponentContext& context)
  {
    auto event = std::make_shared<axgl::Event>();
    event->type = EVENT_TYPE_SEND_NETWORK_MESSAGE;
    event->attributes.insert({ {"message", input_} });

    context.raise_event(std::move(event));
    history_ += input_ + '\n';
    input_.clear();
  }

  void render(axgl::ComponentContext& context) override
  {
    ImGuiWindowFlags window_flags
      = ImGuiWindowFlags_NoTitleBar
      | ImGuiWindowFlags_NoScrollbar
      | ImGuiWindowFlags_NoMove
      | ImGuiWindowFlags_NoResize
      | ImGuiWindowFlags_NoCollapse
      | ImGuiWindowFlags_NoNav
      | ImGuiWindowFlags_NoBackground
      ;

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always, ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size, ImGuiCond_Always);

    constexpr auto padding = 10.0f;
    const auto inner_window_width = ImGui::GetWindowSize().x - (padding * 2);
    const auto inner_window_height = ImGui::GetWindowSize().y - (padding * 2);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(padding, padding));

    ImGui::Begin("Console", nullptr, window_flags);
    {
      ImGui::SetNextItemWidth(inner_window_width);
      ImGui::SetKeyboardFocusHere();
      if (ImGui::InputText("##", &input_, ImGuiInputTextFlags_EnterReturnsTrue))
        on_enter(context);

      ImGui::Text(history_.c_str());
    }
    ImGui::End();
    ImGui::PopStyleVar();
  }
};
