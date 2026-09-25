# 1. Window

The `WindowService` / `Window` interface covers OS window creation and
management. It is the first service most applications use, and the only one
required to display anything at all.

## The Window interface

```cpp
class Window {
public:
  virtual void set_title(const std::string& title) = 0;
  virtual void set_position(std::uint32_t x, std::uint32_t y) = 0;
  virtual void set_size(std::uint32_t width, std::uint32_t height) = 0;
  virtual void maximize() = 0;
  virtual void hide() = 0;
  virtual void show() = 0;
  virtual void close() = 0;
  [[nodiscard]] virtual bool ready() const = 0;
  virtual void use() const = 0;
  virtual void swap_buffers() const = 0;
  [[nodiscard]] virtual glm::ivec2 get_size() const = 0;
};
```

`ready()` reports whether the window is still alive; when it returns false the
engine loop ends. Rendering backends call `use()` to make the window current and
`swap_buffers()` to present a frame.

## Creating a window

```cpp
axgl::Axgl axgl;
axgl::configure_default(axgl);

axgl.initialize();

const auto window = axgl.window_service()->create_window();
window->set_title("Hello Window!");

axgl.run();
axgl.terminate();
```

The window service must be registered before `initialize()`; a window backend
supplies it. `configure_default` is optional and only registers the core
services — see the lifecycle notes in the [documentation index](README.md).

A window by itself does not draw. To render into it, attach a `Renderer` to the
window and build a scene — see [2. Triangle](02_triangle.md) and
[4. Cube](04_cube.md).

Example: `demo/01_window`.
