#include <axgl/components/opengl.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_stdlib.h>


class Playground :
  public axgl::Component,
  public axgl::glfw::Window::EventListener
{
  axgl::OpenglWindow& opengl_window_;

public:
  Playground(axgl::OpenglWindow& opengl_window) :
    opengl_window_(opengl_window) {}

  void initialize() override
  {
    opengl_window_.window()->set_event_listener(this);
    opengl_window_.window()->set_title("Playground");
  }

  bool alive() override { return true; }
};

class UI : public axgl::Component
{
  std::string input_;

public:
  void test()
  {
    SPDLOG_INFO(input_);
  }

  void render() override
  {
    ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoTitleBar |
      ImGuiWindowFlags_NoScrollbar |
      ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoNav |
      ImGuiWindowFlags_NoBackground
      ;

    ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetMainViewport()->Size.y - 200), ImGuiCond_Always, ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_Always);

    ImGui::Begin("Console", nullptr, window_flags);
    auto enter = ImGui::InputText("", &input_,
      ImGuiInputTextFlags_EnterReturnsTrue |
      ImGuiInputTextFlags_EscapeClearsAll |
      ImGuiInputTextFlags_CallbackCompletion |
      ImGuiInputTextFlags_CallbackHistory, [](ImGuiInputTextCallbackData* data) {
        ((UI*)data->UserData)->test();
        return 0;
      }, this);
    if (enter)
      SPDLOG_INFO(input_);
    ImGui::End();
  }

  bool alive() override { return true; }
};

int main()
{
  // gameloop
  axgl::Gameloop gameloop;

  // window
  axgl::OpenglWindow window;
  gameloop.add_component(&window);

  // imgui
  axgl::OpenglImgui imgui(window);
  window.add_component(&imgui);

  // game
  Playground playground(window);
  window.add_component(&playground);

  // ui
  UI ui;
  imgui.add_component(&ui);

  // run
  gameloop.run();
}
