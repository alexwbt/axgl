#pragma once

#include <axgl/interface/context.hpp>

namespace axgl
{

class Service : public axgl::ContextHolder
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
  virtual void exec(const std::vector<std::string>& args) { }
};

} // namespace axgl
