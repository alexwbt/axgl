#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/context.hpp>

namespace axgl
{

class Service : virtual public axgl::ContextHolder
{
public:
  virtual void initialize() { }
  virtual void terminate() { }
  virtual void on_start() { }
  virtual void on_end() { }
  virtual void tick() { }
  virtual void update() { }
  virtual void render() { }
  virtual bool running() { return true; }
  virtual bool keep_alive() { return false; }
  virtual void exec(const std::vector<std::string>&) { }
  [[nodiscard]] virtual std::int32_t priority() const { return 0; }
};

} // namespace axgl
