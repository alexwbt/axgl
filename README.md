# axgl

A C++ game development framework. It defines a set of interfaces for a
game engine, with a collection of implementations.

## Architecture

- **Service-oriented architecture.** The framework is a collection of
  services, each an abstract interface with lifecycle hooks for
  initialization, teardown, and per-frame tick / update / render.
  - **Loose coupling.** Services communicate only through abstract
    interfaces, never concrete implementations — the core has no
    compile-time dependency on a specific backend.
  - **Service autonomy.** Each service is self-contained and independently
    controls the functionality it encapsulates.
  - **Service composability.** Services can be composed of and call upon
    other services.
  - **Service discovery.** Services are registered and resolved at runtime
    through the framework.
- **Component-based entity hierarchy.** The simulation lives in a tree of
  realms and entities. A realm holds entities; an entity holds child
  entities and components. The same tick / update / render rhythm that
  drives services also drives entities and components each frame.
  - **Component pattern.** Composition over inheritance — components are
    pluggable units of behavior attached to entities, driven by their
    parent entity's lifecycle.
  - **Scene graph.** The entity hierarchy is first-class: transforms
    propagate down the tree, so moving a parent moves its children.
- **Interface-based architecture.** Interfaces and implementations are
  decoupled — the core depends only on abstract interfaces.
  - **Design by contract.** Interfaces act as contracts between the core
    and backends — implementers conform to the interface, the core promises
    stability.
  - **Factory pattern.** The framework obtains instances through
    registration rather than direct construction.
  - **Extensibility.** New backends can be provided by anyone satisfying
    the interfaces, without modifying the core.
