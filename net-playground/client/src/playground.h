#pragma once

class Playground :
  public axgl::Component,
  public axgl::glfw::Window::EventListener,
  public std::enable_shared_from_this<Playground>
{
  std::shared_ptr<axgl::OpenglWindow> opengl_window_;

  Playground(std::shared_ptr<axgl::OpenglWindow> opengl_window) :
    opengl_window_(std::move(opengl_window)) {}

public:
  static auto make_shared(std::shared_ptr<axgl::OpenglWindow> opengl_window)
  {
    return std::shared_ptr<Playground>(new Playground(std::move(opengl_window)));
  }

  void initialize(axgl::ComponentContext& context) override
  {
    if (auto window = opengl_window_->window().lock())
    {
      window->set_event_listener(shared_from_this());
      window->set_title("Playground");
    }
  }
};
