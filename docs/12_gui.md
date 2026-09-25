# 12. GUI

The GUI system is a small, CSS-inspired layer built on top of the service
architecture. It is made of a `GuiService`, a tree of `Page` / `Element`
objects, and a named `Style` system with state pseudo-classes.

- Interfaces live in `axgl/include/axgl/interface/gui/` and
  `axgl/include/axgl/interface/services/gui_service.hpp`.

## Core types

| Type                 | Header                               | Role                                                           |
| -------------------- | ------------------------------------ | -------------------------------------------------------------- |
| `axgl::GuiService`   | `interface/services/gui_service.hpp` | Creates pages/elements/styles; owns the main UI                |
| `axgl::gui::Page`    | `interface/gui/page.hpp`             | Root of a UI tree; holds top-level elements and input bindings |
| `axgl::gui::Element` | `interface/gui/element.hpp`          | Base UI node; style, geometry, state, lifecycle                |
| `axgl::gui::Style`   | `interface/gui/style.hpp`            | A set of optional presentation properties                      |
| `axgl::gui::Layout`  | `interface/gui/layout.hpp`           | Positions children of a container                              |
| `axgl::gui::Context` | `interface/gui/context.hpp`          | Per-call context (service, page, parent, scale, projection)    |

`Context` carries the `GuiService*`, the current `Page*`, the parent element,
`scale` / `font_scale`, and the projection matrix. It is rebuilt and passed down
the tree on every lifecycle call.

## GuiService

Defined in `interface/services/gui_service.hpp`. Key operations:

- `create_page()` — make a new `Page`.
- `create_element()` / `create_element(type_id)` — make an element, optionally
  by registered type id.
- `register_element_factory(type_id, fn)` / `register_element_t<T>()` — register
  a factory for a concrete element type.
- `create_style(name)` — create (or replace) a named style and return it.
- `get_style(name)` — look up a named style; returns `nullptr` if absent.
- `set_main_ui(page)` / `get_main_ui()` — install the page that the service
  drives each frame.

The service drives the main page's lifecycle and resets per-frame style
modification tracking between updates. In debug builds, requesting a missing
element id or style name is reported.

## Pages

A `Page` is the root container and the boundary with the rest of the engine. It
owns:

- The element list: `elements()` returns a `Container<Element>&` (add / remove /
  remove_all).
- Size and scale: `set_size`, `set_scale`, plus `get_width` / `get_height`.
- A render target: `get_texture()` and a `should_render()` dirty flag.
- Input bindings:
  - `set_cursor_pointer` / `get_cursor_pointer`
  - `set_scroll_pointer` / `get_scroll_pointer`
  - `set_scale_input` / `get_scale_input`
  - `set_activate_input` / `get_activate_input`
  - `set_focus_switch_input` / `get_focus_switch_input`
  - `set_focus_activate_input` / `get_focus_activate_input`
- `set_cursor` / `get_cursor` for the active cursor.

Wiring a page:

```cpp
const auto page = gui_service->create_page();
page->set_cursor_pointer(cursor_pointer);
page->set_scroll_pointer(scroll_pointer);
page->set_scale_input(scale_input);
page->set_activate_input(activate_input);
page->set_focus_activate_input(focus_activate_input);
page->set_focus_switch_input(focus_switch_input);
gui_service->set_main_ui(page);
```

The service calls `init()` once, then `update()` each frame; `render()` draws
when `should_render()` is set. Any element state change is expected to mark the
page for re-render.

## Elements

`Element` (`interface/gui/element.hpp`) exposes:

- Identity and geometry: `get_id`, `get_position`, `get_size`, `get_rect`,
  `get_visible_rect`, `get_intrinsic_size`.
- State: `is_focusable`, `is_focused`, `is_hovering`, `is_activated`.
- Style: `get_computed_style()` (the resolved style) and `style()` (the
  element's own inline style).
- Children: `children()` returns a `Container<Element>&`.
- Lifecycle: `init`, `update`, `render`.
- Events: `on_pointer_enter`, `on_pointer_exit`, `on_activate`, `on_deactivate`,
  `on_focus`, `on_blur`.
- Geometry setters: `set_position`, `set_size`.
- Style management: `set_style(names)`, `append_style(name)`,
  `remove_style(name)`.

`set_style` replaces the element's style name list and returns a pointer to the
element's inline style, so it can be chained:

```cpp
e->set_style({"text", "h1"})->set_color(axgl::colors::kRed);
```

### Built-in element types

- `axgl::gui::TextElement` (`kTypeId = "element:text"`) — `set_text` /
  `get_text`.
- `axgl::gui::ButtonElement` (`kTypeId = "element:button"`) — `label()` and
  `on_click(handler)`.
- `axgl::gui::InputElement` — currently an empty interface.

Register additional element types with `register_element_t<T>()`.

### Creating elements

```cpp
const auto e = gui_service->create_element_t<axgl::gui::TextElement>();
e->set_text("Hello World");
e->set_style({"text", "h1"});
page->elements().add(e);
```

## Styles

Styling is CSS-inspired: named styles are defined once, referenced by elements,
and combined into a per-element _computed style_.

`axgl::gui::Style` (`interface/gui/style.hpp`) exposes a property as a `get_x()`
/ `set_x()` pair plus a `using_x()` query. The `using_x` flag records whether
the property was explicitly set.

| Property      | Type             | Default        |
| ------------- | ---------------- | -------------- |
| `position`    | `glm::vec2`      | `{0, 0}`       |
| `size`        | `glm::vec2`      | `{0, 0}`       |
| `color`       | `glm::vec4`      | `{0, 0, 0, 0}` |
| `opacity`     | `float`          | `1.0`          |
| `cursor`      | `Cursor`         | `kNormal`      |
| `fonts`       | `vector<string>` | empty          |
| `font_color`  | `glm::vec4`      | `{1, 1, 1, 1}` |
| `font_size`   | `float`          | `16.0`         |
| `font_weight` | `int`            | `400`          |
| `line_height` | `float`          | `1.5`          |
| `text_align`  | `TextAlign`      | `kCenter`      |
| `display`     | `Display`        | `kBlock`       |
| `margin`      | `glm::vec4`      | `{0, 0, 0, 0}` |
| `padding`     | `glm::vec4`      | `{0, 0, 0, 0}` |

Enums: `Display { kBlock, kInline }`, `Cursor { ... }`,
`TextAlign { kLeft, kRight, kCenter }`.

### Named styles

Register a style through the service, then attach it to elements by name:

```cpp
gui_service->create_style("text")->set_fonts({"arial", "noto-tc"});

gui_service->create_style("h1")
  ->set_display(axgl::gui::Display::kBlock)
  ->set_font_size(32.0f)
  ->set_font_weight(700)
  ->set_margin(glm::vec4(10.0f));

gui_service->create_style("p")
  ->set_display(axgl::gui::Display::kBlock)
  ->set_margin(glm::vec4(10.0f));

e->set_style({"text", "h1"});
```

Re-creating an existing name replaces it. `append_style` / `remove_style` adjust
the list incrementally.

### State pseudo-classes

An element automatically also resolves the `:hover`, `:active`, and `:focus`
variants of every name in its style list. Define them under the suffixed name:

```cpp
gui_service->create_style("button:hover")->set_color(axgl::colors::kGray);
gui_service->create_style("button:active")->set_color(axgl::colors::kDarkGray);
gui_service->create_style("button:focus")->set_color(axgl::colors::kBlue);
```

Pseudo-class styles do not have to exist; missing ones are ignored.

### Precedence and computed styles

Resolution order:

1. Start from an empty `Style`.
2. Apply each resolved named style in list order.
3. Apply the element's own inline style (`element->style()`).
4. Publish the result as the _computed style_.

Only properties whose `using_` flag is set are copied during application, so a
later style overrides an earlier one and inline setters override everything.
Within one element the pseudo-class variant of a name is applied right after the
base name, so it wins over the base but is still overridden by later names.

Style recomputation is lazy: it happens when the element's name list or state
changed, its inline style was modified, or a contributing named style was
modified.

### Inline overrides

```cpp
e->set_style({"text", "h1"})->set_color(axgl::colors::kRed);
e->style()->set_font_size(20.0f);
```

Read from `get_computed_style()` when rendering; `style()` is only the inline
layer.

## Layout

`axgl::gui::Layout` (`interface/gui/layout.hpp`) applies positioning to a
container's children:

```cpp
virtual void apply(
  const axgl::gui::Context& context,
  axgl::Container<axgl::gui::Element>& element
) const = 0;
```

A page applies a layout to its top-level elements before running their
lifecycle. `intrinsic_size` is provided by each element and is used by layouts
to size content.

## Writing a custom element

1. Define an interface for the element (optional), deriving from
   `axgl::gui::Element`.
2. Give it a `static constexpr std::string_view kTypeId`.
3. Implement the lifecycle, state, and style accessors required by `Element`.
4. Register it: `gui_service()->register_element_t<MyElement>();`
5. Create it with `gui_service->create_element_t<MyElement>()` or by type id.

For style-driven behavior, read from `get_computed_style()` rather than
`style()`.

Example: `demo/12_gui`.

