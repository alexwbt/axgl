#pragma once

namespace axgl
{

class Axgl;

struct Context
{
  axgl::Axgl* axgl = nullptr;

  double delta_tick = 0.0;
  float delta_tick_f = 0.0f;
};

class ContextHolder
{
public:
  virtual ~ContextHolder() = default;
  virtual void set_context(const Context* context) = 0;
};

} // namespace axgl
