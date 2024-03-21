#pragma once

#include "event.h"
#include "console/command.hpp"

namespace console
{

  class ConnectCommand : public Command
  {
  public:
    void run(
      axgl::ComponentContext& context,
      const std::vector<std::string>& tokens
    )
    {
      auto event = std::make_shared<axgl::Event>();
      event->type = EVENT_TYPE_CONNECT_SERVER;
      event->attributes.set("host", tokens[1]);
      event->attributes.set<uint32_t>("port", std::stoi(tokens[2]));
      context.raise_event(std::move(event));
    }
  };

  class DisconnectCommand : public Command
  {
  public:
    void run(
      axgl::ComponentContext& context,
      const std::vector<std::string>& tokens
    )
    {
      auto event = std::make_shared<axgl::Event>();
      event->type = EVENT_TYPE_DISCONNECT_SERVER;
      context.raise_event(std::move(event));
    }
  };

}
