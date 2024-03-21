#pragma once

#define BASE_EVENT_TYPE 0

#define EVENT_TYPE_WINDOW_EVENT_BASE (BASE_EVENT_TYPE + 1)
#define EVENT_TYPE_WINDOW_KEYDOWN EVENT_TYPE_WINDOW_EVENT_BASE
#define EVENT_TYPE_WINDOW_KEYUP (EVENT_TYPE_WINDOW_KEYDOWN + 1)
#define EVENT_TYPE_WINDOW_MOUSEDOWN (EVENT_TYPE_WINDOW_KEYUP + 1)
#define EVENT_TYPE_WINDOW_MOUSEUP (EVENT_TYPE_WINDOW_MOUSEDOWN + 1)
#define EVENT_TYPE_WINDOW_MOUSEMOVE (EVENT_TYPE_WINDOW_MOUSEUP + 1)
#define EVENT_TYPE_WINDOW_RESIZE (EVENT_TYPE_WINDOW_MOUSEMOVE + 1)
#define EVENT_TYPE_WINDOW_END EVENT_TYPE_WINDOW_RESIZE

#define EVENT_TYPE_CONSOLE_LOG (EVENT_TYPE_WINDOW_END + 1)

#define EVENT_TYPE_CONNECT_SERVER (EVENT_TYPE_CONSOLE_LOG + 1)
#define EVENT_TYPE_DISCONNECT_SERVER (EVENT_TYPE_CONNECT_SERVER + 1)
#define EVENT_TYPE_SEND_NETWORK_MESSAGE (EVENT_TYPE_DISCONNECT_SERVER + 1)


#include <memory>
#include <string>
#include <axgl/component.h>

namespace event
{

  inline void console_log(axgl::ComponentContext& context, const std::string& message)
  {
    auto event = std::make_shared<axgl::Event>(EVENT_TYPE_CONSOLE_LOG);
    event->attributes.set("message", message);
    context.raise_event(std::move(event));
  }

  inline void network_message(axgl::ComponentContext& context, const std::string& message)
  {
    auto event = std::make_shared<axgl::Event>(EVENT_TYPE_SEND_NETWORK_MESSAGE);
    event->attributes.set("message", message);
    context.raise_event(std::move(event));
  }

  inline void network_connect(axgl::ComponentContext& context, const std::string& host, uint32_t port)
  {
    auto event = std::make_shared<axgl::Event>(EVENT_TYPE_CONNECT_SERVER);
    event->attributes.set("host", host);
    event->attributes.set<uint32_t>("port", port);
    context.raise_event(std::move(event));
  }

  inline void network_disconnect(axgl::ComponentContext& context)
  {
    context.raise_event(std::make_shared<axgl::Event>(EVENT_TYPE_DISCONNECT_SERVER));
  }

}
