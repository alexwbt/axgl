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
  struct Context final
  {
    Axgl* axgl = nullptr;
  };

  virtual ~Service() = default;

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

  virtual void set_context(const Context* context) = 0;

protected:
  virtual const Context* get_context() const = 0;
};

} // namespace axgl
