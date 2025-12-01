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
  };

private:
  static std::uint32_t next_id()
  {
    static std::uint32_t next_id_ = 1;
    return next_id_++;
  }

public:
  const std::uint32_t id;
  std::string name;
  axgl::Pointer::Source source;
  glm::ivec2 position;
  glm::ivec2 delta;
  std::uint32_t tick;

  Pointer(std::string name, const axgl::Pointer::Source source) :
    id(next_id()), name(std::move(name)), source(source), position(0), delta(0), tick(0)
  {
  }
};

} // namespace axgl
