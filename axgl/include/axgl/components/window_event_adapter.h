#pragma once

#include <deque>
#include <stdint.h>

#include "axgl/component.h"

NAMESPACE_AXGL

class WindowEventAdapter :
  public Component,
  public glfw::Window::EventListener
{
private:
  uint32_t base_event_type_value_;
  std::deque<std::shared_ptr<Event>> new_events_;

public:
  WindowEventAdapter(uint32_t base_event_type_value)
    : base_event_type_value_(base_event_type_value) {}

  void update(ComponentContext& context) override
  {
    for (const auto& event : new_events_)
      context.raise_event(std::move(event));

    new_events_.clear();
  }

  void on_key_down(int key) override
  {
    auto event = std::make_shared<Event>(base_event_type_value_);
    event->attributes.set("key", key);
    new_events_.push_back(std::move(event));
  }

  void on_key_up(int key) override
  {
    auto event = std::make_shared<Event>(base_event_type_value_ + 1);
    event->attributes.set("key", key);
    new_events_.push_back(std::move(event));
  }

  void on_mouse_down(int button) override
  {
    auto event = std::make_shared<Event>(base_event_type_value_ + 2);
    event->attributes.set("button", button);
    new_events_.push_back(std::move(event));
  }

  void on_mouse_up(int button) override
  {
    auto event = std::make_shared<Event>(base_event_type_value_ + 3);
    event->attributes.set("button", button);
    new_events_.push_back(std::move(event));
  }

  void on_mouse_move(double x, double y) override
  {
    auto event = std::make_shared<Event>(base_event_type_value_ + 4);
    event->attributes.set("x", x);
    event->attributes.set("y", y);
    new_events_.push_back(std::move(event));
  }

  void on_resize(int width, int height) override
  {
    auto event = std::make_shared<Event>(base_event_type_value_ + 5);
    event->attributes.set("width", width);
    event->attributes.set("height", height);
    new_events_.push_back(std::move(event));
  }
};

NAMESPACE_AXGL_END
