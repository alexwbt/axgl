
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
