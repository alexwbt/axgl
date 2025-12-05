#pragma once

#include <vector>

namespace axgl
{
class Axgl;
}

namespace axgl
{

class Service
{
public:
  struct Context
  {
    Axgl& axgl;
    double delta_tick;
  };

  virtual ~Service() = default;
  virtual void initialize(const Context& context) { }
  virtual void terminate(const Context& context) { }
  virtual void on_start(const Context& context) { }
  virtual void on_end(const Context& context) { }
  virtual void tick(const Context& context) { }
  virtual void update(const Context& context) { }
  virtual void render(const Context& context) { }
  virtual bool running(const Context& context) { return true; }
  virtual bool keep_alive(const Context& context) { return false; }
  virtual void exec(const Context& context, const std::vector<std::string>& args) { }
};

} // namespace axgl
