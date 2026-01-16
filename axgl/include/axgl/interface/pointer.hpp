#pragma once

#include <axgl/common.hpp>

namespace axgl
{

class Pointer final
{
public:
  enum class Source
  {
    kMouseMove,
    kScroll,
  };

private:
  static std::uint64_t next_id()
  {
    static std::uint64_t next_id_ = 1;
    return next_id_++;
  }

public:
  const std::uint64_t id;
  std::string name;
  glm::vec2 position;
  glm::vec2 delta;
  std::uint64_t tick;
  axgl::Pointer::Source source;

  Pointer(const std::string& name, const axgl::Pointer::Source source) :
    id(next_id()), name(name), position(0.0f), delta(0.0f), tick(0), source(source)
  {
  }
};

} // namespace axgl
