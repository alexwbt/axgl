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
| `axgl::gui::Layout`  | `interface/gui/layout.hpp`           | Measures and positions children of a container                 |
| `axgl::gui::Context` | `interface/gui/context.hpp`          | Per-call context (service, page, parent, scale, projection)    |

`Context` carries the `GuiService*`, the current `Page*`, the parent element,
`scale` / `font_scale`, and the projection matrix. It is rebuilt and passed down
the tree on every lifecycle call.

## GuiService

Defined in `interface/services/gui_service.hpp`. Key operations:

- `create_page()` — make a new `Page`.
- `create_element()` / `create_element(type_id)` — make an element, optionally
  by registered type id. If a style named after the part of the type id following
  `element:` (`text`, `button`, ...) exists, it is appended to the new element
  automatically.
- `create_element_t<T>(default_style = {})` — typed convenience wrapper. When
  `default_style` is non-empty, it replaces the element's style name list with
  those names.
- `register_element_factory(type_id, fn)` / `register_element_t<T>()` — register
  a factory for a concrete element type.
- `create_style(name)` — create (or replace) a named style and return it.
- `get_style(name)` — look up a named style; returns `nullptr` if absent.
- `set_main_ui(page)` / `get_main_ui()` — install the page that the service
  drives each frame.

The service drives the main page's lifecycle and resets per-frame style
modification tracking between updates. In debug builds, requesting a missing
element id or style name is reported.

The default GUI service installs a set of built-in named styles: `text` (the font
base), `header1` and `paragraph` (block styles based on `text`), and `button`
together with the `button:hover` / `button:active` pseudo-class variants.

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

Focus is page-driven. Each frame the page collects the focusable elements in
tree order (an element opts in with `set_focusable(true)`; buttons and inputs do
so by default). `focus_switch_input` (e.g. Tab) advances focus to the next one,
wrapping around, and calls `on_blur` on the old element and `on_focus` on the
new one. `focus_activate_input` (e.g. Enter) calls `on_activate` on the focused
element, which is how a focused button fires its click handler.

## Elements

`Element` (`interface/gui/element.hpp`) exposes:

- Identity and geometry: `get_id`, `get_position` (absolute), `get_offset`
  (relative to the parent), `get_size`, `get_desired_size` (the result of the
  last measure), `get_rect`, `get_visible_rect`.
- State: `is_focusable`, `is_focused`, `is_hovering`, `is_activated`.
- Style: `get_computed_style()` (the resolved style) and `style()` (the
  element's own inline style).
- Children: `children()` returns a `Container<Element>&`, plus `get_layout()` /
  `set_layout()` for the layout applied to those children.
- Lifecycle: `init`, `update`, `render`.
- Measurement: `measure(context, available_size)` and `arrange(context, offset,
  size)`, called by the parent layout (see [Layout](#layout)).
- Events: `on_pointer_enter`, `on_pointer_exit`, `on_activate`, `on_deactivate`,
  `on_focus`, `on_blur`.
- Style management: `set_style(names)`, `append_style(name)`,
  `remove_style(name)`.

`get_id` is a process-unique id assigned when the element is created. `get_rect`
is the element's own rect in page coordinates; `get_visible_rect` additionally
intersects it with its ancestors' rects, so rendering clips to it and nested or
composite widgets clip cleanly.

`set_style` replaces the element's style name list and returns a pointer to the
element's inline style, so it can be chained:

```cpp
e->set_style({"text", "header1"})->set_color(axgl::colors::kRed);
```

### Built-in element types

- `axgl::gui::TextElement` (`kTypeId = "element:text"`) — `set_text` /
  `get_text`.
- `axgl::gui::ButtonElement` (`kTypeId = "element:button"`) — `label()` and
  `on_click(handler)`. Focusable by default.
- `axgl::gui::InputElement` (`kTypeId = "element:input"`) — a focusable,
  single-line text field. Inherits `TextElement` (`set_text` / `get_text`) and
  adds `on_submit(handler)` for Enter. While focused it consumes the
  `InputService` text-edit events (typing, backspace, delete, arrow keys,
  Enter) and draws a caret.

Register additional element types with `register_element_t<T>()`.

### Creating elements

```cpp
const auto e = gui_service->create_element_t<axgl::gui::TextElement>({"header1"});
e->set_text("Hello World");
page->elements().add(e);
```

`create_element_t<T>()` with no arguments applies whatever default style matches
the element's type id, if one exists (for example `text` for `TextElement`,
`button` for `ButtonElement`). Passing a name list overrides that choice.

## Styles

Styling is CSS-inspired: named styles are defined once, referenced by elements,
and combined into a per-element _computed style_.

`axgl::gui::Style` (`interface/gui/style.hpp`) exposes a property as a `get_x()`
/ `set_x()` pair plus a `using_x()` query. The `using_x` flag records whether
the property was explicitly set. A property that was not set is read from the
style's base style, if one is set (see [Base styles](#base-styles)).

| Property     | Type             | Default        |
| ------------ | ---------------- | -------------- |
| `color`      | `glm::vec4`      | `{0, 0, 0, 0}` |
| `opacity`    | `float`          | `1.0`          |
| `cursor`     | `Cursor`         | `kNormal`      |
| `fonts`      | `vector<string>` | empty          |
| `font_color` | `glm::vec4`      | `{1, 1, 1, 1}` |
| `font_size`  | `float`          | `16.0`         |
| `display`    | `Display`        | `kBlock`       |
| `margin`     | `glm::vec4`      | `{0, 0, 0, 0}` |
| `padding`    | `glm::vec4`      | `{0, 0, 0, 0}` |

`margin` and `padding` use the CSS shorthand order `(top, right, bottom, left)`.
Element geometry is owned by the layout, not by styles: an element's position and
size are assigned during arrange. Use `margin` / `padding` / `display` to
influence it.

Enums: `Display { kBlock, kInline }`, `Cursor { ... }`.

### Named styles

Register a style through the service, then attach it to elements by name:

```cpp
gui_service->create_style("text")->set_fonts({"arial", "noto-tc"});

gui_service->create_style("header1")
  ->set_base_style(gui_service->get_style("text"))
  ->set_display(axgl::gui::Display::kBlock)
  ->set_font_size(32.0f)
  ->set_margin(glm::vec4(10.0f));

gui_service->create_style("paragraph")
  ->set_base_style(gui_service->get_style("text"))
  ->set_display(axgl::gui::Display::kBlock)
  ->set_margin(glm::vec4(10.0f));

e->set_style({"header1"});
```

Re-creating an existing name replaces it. `append_style` / `remove_style` adjust
the list incrementally.

### Base styles

A style can point at another style with `set_base_style(style)`. Reading a
property that was not set locally (`using_x()` is false) falls back to the base
style's value, so a base can supply shared defaults such as `fonts` while the
derived style overrides only what differs. The base is held by shared ownership,
so it stays alive as long as a derived style references it. Assigning a base that
would form a cycle (`a -> b -> a`) is detected and ignored.

```cpp
const auto text = gui_service->create_style("text");
text->set_fonts({"arial", "noto-tc"});

gui_service->create_style("header1")
  ->set_base_style(text)
  ->set_font_size(32.0f);
```

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

Only properties the style sets are copied during application. A property not
set locally is copied from the style's base chain (its resolved value), so a
later style overrides an earlier one and inline setters override everything.
Within one element the pseudo-class variant of a name is applied right after the
base name, so it wins over the base but is still overridden by later names.

Style recomputation is lazy: it happens when the element's name list or state
changed, its inline style was modified, or a contributing named style (or a
style it inherits from) was modified.

### Inline overrides

```cpp
e->set_style({"text", "header1"})->set_color(axgl::colors::kRed);
e->style()->set_font_size(20.0f);
```

Read from `get_computed_style()` when rendering; `style()` is only the inline
layer.

## Layout

`axgl::gui::Layout` (`interface/gui/layout.hpp`) drives a container's children
in two phases:

```cpp
virtual glm::vec2 measure(
  const axgl::gui::Context& context,
  axgl::Container<axgl::gui::Element>& elements,
  const glm::vec2& available_size
) const = 0;

virtual void arrange(
  const axgl::gui::Context& context,
  axgl::Container<axgl::gui::Element>& elements,
  const glm::vec2& content_origin,
  const glm::vec2& available_size
) const = 0;
```

- **Measure** is top-down constraints, bottom-up sizes: the layout asks each
  child to `measure` itself and returns the container's content size. Each
  element records its own `get_desired_size()` during this pass.
- **Arrange** is top-down placement: the layout positions each child with a
  parent-relative `offset` and a final `size`. `content_origin` is the top-left
  of the container's content box (i.e. inside its padding).

Layout is **recursive and owned per container**. Both `Page` and `Element`
expose `get_layout()` / `set_layout()`; the default is `BlockLayout`. A page
measures and arranges its top-level elements, and every element arranges its own
children with its layout, so nesting works without special cases. The page
lifecycle is `init` (or `update`) → `measure` → `arrange`, then `render`.

Coordinate resolution is explicit: an element stores a parent-relative
`offset_` and an absolute `position_`, resolved once during arrange. There is no
in-place accumulation, so geometry is stable frame-to-frame.

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

