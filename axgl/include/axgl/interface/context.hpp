#pragma once

namespace axgl
{

class Axgl;

struct Context
{
  axgl::Axgl* axgl;

  double delta_tick;
  float delta_tick_f;
};

class ContextHolder
{
public:
  virtual ~ContextHolder() = default;
  virtual void set_context(const Context* context) = 0;
};

} // namespace axgl
