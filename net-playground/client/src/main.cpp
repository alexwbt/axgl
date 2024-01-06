#include <axgl/components/opengl.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

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

  bool alive() override
  {
    return true;
  }
};

class ImGuiDemo : public axgl::Component
{
public:
  void render() override
  {
    ImGui::ShowDemoWindow();
  }

  bool alive() override
  {
    return true;
  }
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

  // imgui demo
  ImGuiDemo demo;
  imgui.add_component(&demo);

  // run
  gameloop.run();
}
