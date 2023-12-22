#pragma once

#include <vector>
#include <memory>

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

class Gameloop : public Component
{
  std::vector<Component*> components_;

public:
  // does not take ownership of component
  void add_component(Component* component);

  void initialize() override;
  void terminate() override;
  void update() override;
  void render() override;
  bool alive() override;

  void run();
};

NAMESPACE_AXGL_END
