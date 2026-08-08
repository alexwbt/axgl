#pragma once

namespace axgl::gui
{

class Element;

class Layout
{
public:
  virtual ~Layout() = default;

  virtual void apply(axgl::gui::Element& element) const = 0;
};

} // namespace axgl::gui
