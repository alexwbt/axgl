#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <ranges>
#include <deque>

#include "axgl/namespace.h"


NAMESPACE_AXGL

struct Event
{
  int consumed = 0;
  uint32_t type = 0;
  bool keep_alive = false;
  std::unordered_map<std::string, std::string> attributes;
};

class ComponentContext
{
  std::unordered_map<std::string, std::string> attributes_;

  std::vector<std::shared_ptr<Event>> new_events_;
  std::unordered_map<uint32_t, std::deque<std::shared_ptr<Event>>> events_;

public:
  void raise_event(std::shared_ptr<Event> event)
  {
    new_events_.push_back(std::move(event));
  }

  auto get_events(uint32_t type)
  {
    return events_[type];
  }

  void update()
  {
    for (auto& [type, events] : events_)
    {
      std::erase_if(events, [](std::shared_ptr<Event> event)
      {
        return !event->keep_alive && event->consumed > 0;
      });
    }

    if (!new_events_.empty())
    {
      for (const auto& event : new_events_)
        events_[event->type].push_back(event);

      new_events_.clear();
    }
  }
};

struct Component
{
  virtual ~Component() {}
  virtual void initialize(ComponentContext& context) {}
  virtual void terminate(ComponentContext& context) {}
  virtual void update(ComponentContext& context) {}
  virtual void render(ComponentContext& context) {}
  virtual bool alive(ComponentContext& context) { return true; }
};

class ComponentParent : public Component
{
  std::vector<std::shared_ptr<Component>> components_;

public:
  void add_component(std::shared_ptr<Component> component)
  {
    components_.push_back(std::move(component));
  }

  void initialize(ComponentContext& context) override
  {
    for (const auto& component : components_)
      component->initialize(context);
  }

  void terminate(ComponentContext& context) override
  {
    for (auto it = components_.rbegin(); it != components_.rend(); ++it)
      (*it)->terminate(context);
  }

  void update(ComponentContext& context) override
  {
    for (const auto& component : components_)
      if (component->alive(context))
        component->update(context);
  }

  void render(ComponentContext& context) override
  {
    for (const auto& component : components_)
      if (component->alive(context))
        component->render(context);
  }

  bool alive(ComponentContext& context) override
  {
    for (const auto& component : components_)
      if (component->alive(context))
        return true;
    return false;
  }
};

NAMESPACE_AXGL_END
