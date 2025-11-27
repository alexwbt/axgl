#pragma once

#include <vector>

#include <axgl/interface/container.hpp>
#include <axgl/interface/gui/style.hpp>

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

  virtual Style* style() = 0;
  virtual Container<Element>* children() = 0;
  virtual std::shared_ptr<Element> parent() = 0;
};

template <typename ElementType>
std::shared_ptr<Element> element(const Style& style, const std::vector<std::shared_ptr<Element>> children)
{
  const auto element = std::make_shared<ElementType>();
  *element->style() = style;
  element->children()->add_all(children);
  return element;
}

} // namespace axgl::gui
