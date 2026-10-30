# 7. Input

Input is split between `InputService`, which routes window events, `Input`
objects, which represent a named action or key, and `Pointer` objects, which
track cursor and scroll state. Camera motion is delegated to a `CameraMode`.

## Binding to a window

```cpp
const auto window = axgl.window_service()->create_window();
axgl.input_service()->set_window(window);
```

`InputService` also supports `set_cursor_mode` (`kNormal` or `kLocked`), and
registration of inputs and pointers:

```cpp
virtual void add_input(axgl::ptr_t<axgl::Input> input) = 0;
virtual void add_pointer(axgl::ptr_t<axgl::Pointer> pointer) = 0;
virtual void remove_input(std::uint64_t id) = 0;
virtual void remove_pointer(std::uint64_t id) = 0;
virtual std::vector<ptr_t<Input>> get_input_by_source(Input::Source) = 0;
virtual std::vector<ptr_t<Pointer>> get_pointer_by_source(Pointer::Source) = 0;
```

## Input objects

`Input` is constructed with a name and one or more `Source` values (keyboard
keys or mouse buttons from the `Input::Source` enum):

```cpp
const auto input = axgl::create_ptr<axgl::Input>(
  "Jump", axgl::Input::Source::kKeySpace
);
axgl.input_service()->add_input(input);
```

It exposes per-frame state:

```cpp
input->clicked(); // pressed this frame (tick == 1)
input->down();    // held (tick > 0)
input->up();      // released (tick == 0)
```

`tick` counts how many frames the source has been held, and `sources` lists the
bound keys/buttons. Pointers track position and delta for mouse move and scroll
sources, as used by the [12. GUI](12_gui.md).

## Key events

Besides the per-`Input` state, `InputService` exposes the key presses the window
produced this frame, refreshed on every update:

```cpp
virtual std::span<const axgl::KeyEvent> get_key_events() const = 0;
```

A `KeyEvent` is modelled on the browser `keydown` event:

```cpp
struct KeyEvent {
  axgl::Input::Source key; // physical key
  char32_t character = 0;  // produced character, 0 if none
};
```

`key` identifies the physical key (reusing the `Input::Source` enum).
`character` is the Unicode character the press produced, already accounting for
shift and keyboard layout; it is `0` for keys that produce no text (Backspace,
arrows, Enter, ...). Keys are reported whether or not they are bound to an
`Input`. See [12. GUI](12_gui.md) for how the focused widget consumes them.

## Camera modes

A `CameraMode` turns input into camera motion. Install one and point it at the
camera entity:

```cpp
axgl.camera_service()->set_camera_mode(
  axgl::create_ptr<Keyboard3DFreeFlyCameraMode>()
);
axgl.camera_service()->set_camera_entity(camera_entity);
```

`CameraService::get_camera()` returns the underlying `axgl::Camera` if the mode
or other code needs to read or write camera state directly. Camera demos across
the tree use a free-fly mode; see [4. Cube](04_cube.md) for the camera entity
setup.

Example: `demo/07_input`.
