#pragma once

#include <string>

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
  axgl::Pointer::Source source;
  glm::vec2 position;
  glm::vec2 delta;
  std::uint64_t tick;

  Pointer(std::string name, const axgl::Pointer::Source source) :
    id(next_id()), name(std::move(name)), source(source), position(0.0f), delta(0.0f), tick(0)
  {
  }
};

} // namespace axgl
