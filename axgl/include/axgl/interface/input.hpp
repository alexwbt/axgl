#pragma once

#include <utility>

#include <axgl/common.hpp>
#include <axgl/interface/input_source.hpp>

namespace axgl {

class Input final {
private:
  static std::uint64_t next_id() {
    static std::uint64_t next_id_ = 1;
    return next_id_++;
  }

public:
  const std::uint64_t id;
  std::string name;
  std::uint64_t tick;
  std::vector<axgl::InputSource> sources;

  Input(std::string name, const std::vector<axgl::InputSource>& sources) :
    id(next_id()), name(std::move(name)), tick(0), sources(sources) {}

  Input(const std::string& name, axgl::InputSource source) :
    Input(name, std::vector{source}) {}

  [[nodiscard]] bool clicked() const { return tick == 1; }
  [[nodiscard]] bool down() const { return tick > 0; }
  [[nodiscard]] bool up() const { return tick == 0; }
};

struct KeyEvent {
  axgl::InputSource key;
  char32_t character = 0;
};

} // namespace axgl
