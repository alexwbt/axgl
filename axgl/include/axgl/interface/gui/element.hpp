#pragma once

#include <memory>

#include <axgl/interface/container.hpp>

namespace axgl::gui
{

class Element
{
public:
  virtual ~Element() = default;

  virtual void tick() { }
  virtual void update() { }
  virtual void render() { }
  virtual void on_create() { }
  virtual void on_remove() { }

  virtual Container<Element>* children() = 0;

  virtual void set_parent(std::weak_ptr<Element> parent) = 0;
  virtual std::shared_ptr<Element> get_parent() = 0;
};

} // namespace axgl::gui
