# Documentation

Topic guides for the axgl public interfaces. Each numbered document covers a
framework topic; the numbering matches the corresponding worked example.

| Topic                                | Summary                            |
| ------------------------------------ | ---------------------------------- |
| [Window](01_window.md)               | Engine lifecycle and windows       |
| [Triangle](02_triangle.md)           | 2D meshes and orthographic cameras |
| [Circle](03_circle.md)               | Indexed and procedural geometry    |
| [Cube](04_cube.md)                   | 3D scenes and custom services      |
| [Texture](05_texture.md)             | Textures and UV mapping            |
| [Textured Cube](06_textured_cube.md) | Material texture slots             |
| [Input](07_input.md)                 | Input, pointers, and camera modes  |
| [Model](08_model.md)                 | Models and resources               |
| [Text](09_text.md)                   | Text rendering                     |
| [Instancing](10_instancing.md)       | Custom entities and instancing     |
| [Blending](11_blending.md)           | Alpha blending materials           |
| [GUI](12_gui.md)                     | Pages, elements, and styles        |

## Engine lifecycle

Every application follows the same lifecycle:

```cpp
axgl::Axgl axgl;
axgl::configure_default(axgl);
axgl.initialize();

// acquire services through axgl.<name>_service() and build the scene

axgl.run();
axgl.terminate();
```

`Axgl` is the entrypoint. Registration happens before `initialize()` and
constructs nothing; `initialize()` constructs and starts every registered
service. After that, `run()` drives a fixed-timestep loop (tick / update /
render) until the application ends, and `terminate()` shuts the services down.

`configure_default` is optional: it is merely a convenience that registers the
core services. Registration is per-service, so an application can hand-pick
exactly the services it needs by registering each one itself, then add more on
top of `configure_default` if it likes.

## Services

The engine is a set of services, each reachable from `Axgl` via a
`<name>_service()` accessor and described by an abstract interface:

- `WindowService` — creates `Window` objects.
- `RendererService` — creates `Renderer`, `Texture`, and `Material` objects.
- `RealmService` — creates `Realm` objects and selects the active one.
- `EntityService` — creates `Entity` objects and `Component` objects.
- `CameraService` — selects the camera entity and its `CameraMode`.
- `InputService` — binds `Input` / `Pointer` sources to a window.
- `TextService` — loads fonts and renders text to a `Texture`.
- `ModelService` — loads model files into meshes, materials, and textures.
- `ResourceService` — holds resource data by key.
- `GuiService` — creates pages, elements, and styles.

Services are registered individually:

```cpp
axgl.register_service_t<MyService>();
```

`configure_default` and the backend `configure_*` functions are just bundles of
such calls. A concrete service type is chosen by the application or supplied by
a backend, so registering a service by hand is equivalent to letting a
convenience function register it.

## Scene model

The simulation lives in a `Realm` containing `Entity` objects. An `Entity` owns
a `Transform` and containers of child entities and `Component` objects. Concrete
types are never referenced directly; everything is reached through the
interfaces above, and objects are shared through `axgl::ptr_t<T>`
(`std::shared_ptr<T>`).
