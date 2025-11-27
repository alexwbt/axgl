#pragma once

#include <string>

namespace axgl::gui
{

class Event
{
public:
  virtual ~Event() = default;

  virtual std::string type() = 0;
};

} // namespace axgl::gui
