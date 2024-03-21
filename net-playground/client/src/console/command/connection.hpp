#pragma once

#include "event.hpp"
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
      if (tokens.size() == 3)
        event::network_connect(context, tokens[1], std::stoi(tokens[2]));
      else
        event::console_log(context, "invalid arguments");
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
      event::network_disconnect(context);
    }
  };

}
