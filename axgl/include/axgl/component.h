#pragma once

#include <vector>

#include "axgl/namespace.h"


NAMESPACE_AXGL

struct Component
{
  virtual ~Component() {}
  virtual void initialize() {}
  virtual void terminate() {}
  virtual void update() {}
  virtual void render() {}
  virtual bool alive() { return false; }
};

class ComponentParent : public Component
{
  std::vector<Component*> components_;

public:
  // does not take ownership of component
  void add_component(Component* component)
  {
    components_.push_back(component);
  }

  void initialize()
  {
    for (const auto& component : components_)
      component->initialize();
  }

  void terminate()
  {
    for (auto it = components_.rbegin(); it != components_.rend(); ++it)
      (*it)->terminate();
  }

  void update()
  {
    for (const auto& component : components_)
      if (component->alive())
        component->update();
  }

  void render()
  {
    for (const auto& component : components_)
      if (component->alive())
        component->render();
  }

  bool alive()
  {
    for (const auto& component : components_)
      if (component->alive())
        return true;
    return false;
  }
};

NAMESPACE_AXGL_END
