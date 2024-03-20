#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <deque>

#include "event.h"

class Console : public axgl::Component
{
  bool show_ = false;
  bool scroll_to_bottom_ = false;

  std::string input_;
  std::deque<std::string> history_;

public:
  void on_enter(axgl::ComponentContext& context)
  {
    if (input_.starts_with("/"))
    {
      std::vector<std::string> tokens;
      std::istringstream iss(input_);
      std::string item;
      while (std::getline(iss, item, ' '))
        tokens.push_back(item);

      if (tokens[0] == "/connect" && tokens.size() == 3)
      {
        auto event = std::make_shared<axgl::Event>();
        event->type = EVENT_TYPE_CONNECT_SERVER;
        event->attributes.set("host", tokens[1]);
        event->attributes.set<uint32_t>("port", std::stoi(tokens[2]));
        context.raise_event(std::move(event));
      }
      else if (tokens[0] == "/disconnect")
      {
        auto event = std::make_shared<axgl::Event>();
        event->type = EVENT_TYPE_DISCONNECT_SERVER;
        context.raise_event(std::move(event));
      }
      else if (tokens[0] == "/clear")
      {
        history_.clear();
      }
    }
    else
    {
      auto event = std::make_shared<axgl::Event>();
      event->type = EVENT_TYPE_SEND_NETWORK_MESSAGE;
      event->attributes.set("message", input_);
      context.raise_event(std::move(event));
    }

    append_history(input_);
    input_.clear();
  }

  void append_history(const std::string& value)
  {
    history_.push_back(input_);
    scroll_to_bottom_ = true;
  }

  void update(axgl::ComponentContext& context) override
  {
    const auto& events = context.get_events(EVENT_TYPE_WINDOW_KEYDOWN);
    for (const auto& event : events)
    {
      const auto& key = event->attributes.get<int>("key");
      if (key == (show_ ? GLFW_KEY_ESCAPE : GLFW_KEY_T))
      {
        show_ = !show_;
        break;
      }
    }
  }

  void render(axgl::ComponentContext& context) override
  {
    if (!show_)
      return;

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
    ImGui::Begin("Console", nullptr, window_flags);
    {
      // History
      ImGui::BeginChild("History",
        ImVec2(0, -ImGui::GetFrameHeightWithSpacing()),
        false, ImGuiWindowFlags_HorizontalScrollbar);
      {
        for (const auto& history : history_)
          ImGui::TextUnformatted(history.c_str());

        if (scroll_to_bottom_)
        {
          ImGui::SetScrollHereY(1.0f);
          scroll_to_bottom_ = false;
        }
      }
      ImGui::EndChild();

      // Input Text
      ImGui::SetKeyboardFocusHere();
      ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

      ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
      if (ImGui::InputText("##", &input_, ImGuiInputTextFlags_EnterReturnsTrue))
        on_enter(context);
      ImGui::PopStyleColor(2);
    }
    ImGui::End();
  }
};
