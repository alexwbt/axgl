
class Console : public axgl::Component
{
  std::string input_;

public:
  void on_enter()
  {
    SPDLOG_INFO(input_);
    input_.clear();
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
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    ImGui::Begin("Console", nullptr, window_flags);
    {
      ImGui::SetNextItemWidth(ImGui::GetWindowSize().x);
      ImGui::SetKeyboardFocusHere();
      if (ImGui::InputText("", &input_, ImGuiInputTextFlags_EnterReturnsTrue))
        on_enter();
    }
    ImGui::End();
  }

  bool alive() override { return true; }
};
