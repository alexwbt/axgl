#pragma once

#include <vector>

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

  virtual std::shared_ptr<Element> parent() = 0;
  virtual void set_parent(std::shared_ptr<Element> parent) = 0;
};

template <typename ElementType>
std::shared_ptr<Element> element(const std::vector<std::shared_ptr<Element>>& children = {})
{
  const auto element = std::make_shared<ElementType>();

  for (const auto child : children)
    child->set_parent(element);

  element->children()->add_all(children);
  return element;
}

} // namespace axgl::gui
