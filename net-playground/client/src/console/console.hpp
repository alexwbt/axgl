#pragma once

#include <util/string.hpp>

#include "event.hpp"
#include "console/command/connection.hpp"

namespace console
{

  class Console : public axgl::Component
  {
    bool show_ = false;
    bool scroll_to_bottom_ = false;
    bool deselect_text_ = false;

    std::string input_;
    std::string tmp_input_;
    int history_cursor_ = 0;
    std::vector<std::string> history_;
    std::vector<std::string> input_history_;

    Commands commands_;

  public:
    Console()
    {
      commands_.put_command("/connect", std::make_shared<ConnectCommand>());
      commands_.put_command("/disconnect", std::make_shared<DisconnectCommand>());
    }

    void reset_history_cursor()
    {
      tmp_input_.clear();
      history_cursor_ = 0;
    }

    void append_history(const std::string& value)
    {
      history_.push_back(value);
      scroll_to_bottom_ = true;
    }

    void on_enter(axgl::ComponentContext& context)
    {
      util::trim(input_);

      if (input_.empty())
        return;

      auto clear = false;

      if (input_.starts_with("/") && input_.size() > 1)
      {
        if (input_ == "/clear")
        {
          clear = true;
        }
        else
        {
          // call a command
          std::vector<std::string> tokens = util::split(input_, ' ');
          commands_.call(context, tokens[0], tokens);
        }
      }
      else
        // sends network message
        event::network_message(context, input_);

      if (!clear)
        append_history(input_);
      else
        history_.clear();
      reset_history_cursor();
      input_history_.push_back(input_);
      input_.clear();
    }

    static int input_text_callback(ImGuiInputTextCallbackData* data)
    {
      auto* this_ = static_cast<Console*>(data->UserData);

      if (this_->deselect_text_)
      {
        data->ClearSelection();
        this_->deselect_text_ = false;
      }

      switch (data->EventFlag)
      {
      case ImGuiInputTextFlags_CallbackHistory:
        if (this_->input_history_.size() == 0)
          return 0;
        if (this_->history_cursor_ == 0)
          this_->tmp_input_ = this_->input_;

        this_->history_cursor_ += data->EventKey == ImGuiKey_UpArrow ? 1 : -1;

        if (this_->history_cursor_ == 0)
          this_->input_ = this_->tmp_input_;
        else if (this_->history_cursor_ < 0)
          this_->history_cursor_ = 0;
        else
        {
          if (this_->history_cursor_ > this_->input_history_.size())
            this_->history_cursor_ = this_->input_history_.size();
          this_->input_ = this_->input_history_[this_->input_history_.size() - this_->history_cursor_];
        }

        data->DeleteChars(0, data->BufTextLen);
        data->InsertChars(0, this_->input_.c_str());
        return 1;

      default:
        return 0;
      }
    }

    void update(axgl::ComponentContext& context) override
    {
      const auto& keydown_events = context.get_events(EVENT_TYPE_WINDOW_KEYDOWN);
      for (const auto& event : keydown_events)
      {
        const auto& key = event->attributes.get<int>("key");
        if (show_)
        {
          if (key == GLFW_KEY_ESCAPE)
          {
            show_ = false;
            reset_history_cursor();
          }
        }
        else if (key == GLFW_KEY_T)
        {
          show_ = true;
        }
        else if (key == GLFW_KEY_SLASH)
        {
          show_ = true;
          input_ = "/";
          deselect_text_ = true;
        }
      }

      const auto& log_events = context.get_events(EVENT_TYPE_CONSOLE_LOG);
      for (const auto& event : log_events)
      {
        const auto& message = event->attributes.get<std::string>("message");
        append_history(message);
      }
    }

    void render(axgl::ComponentContext& context) override
    {
      if (!show_)
        return;

      ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always, ImVec2(0, 0));
      ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size, ImGuiCond_Always);
      ImGui::Begin("Console", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoBackground);
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
        if (ImGui::InputText("##", &input_,
          ImGuiInputTextFlags_EnterReturnsTrue |
          ImGuiInputTextFlags_CallbackCompletion |
          ImGuiInputTextFlags_CallbackHistory |
          ImGuiInputTextFlags_CallbackAlways,
          &Console::input_text_callback, this))
          on_enter(context);
        ImGui::PopStyleColor(2);
      }
      ImGui::End();
    }
  };

}
