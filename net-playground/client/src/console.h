#pragma once

class Console : public axgl::Component
{
  std::string input_;
  std::string history_;

  NetClient client_;
  std::thread client_thread_;

public:
  Console() :
    client_("127.0.0.1", 13000),
    client_thread_([&]()
    {
      client_.start();
    })
  {}

  void on_enter()
  {
    client_.send(create_message(input_));
    history_ += input_ + '\n';
    input_.clear();
  }

  void terminate() override
  {
    client_.stop();
    client_thread_.join();
  }

  void render() override
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
        on_enter();

      ImGui::Text(history_.c_str());
    }
    ImGui::End();
    ImGui::PopStyleVar();
  }

  bool alive() override { return true; }
};
