#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include <axgl/component.h>

namespace console
{

  class Command
  {
  public:
    virtual ~Command() {}

  public:
    virtual void run(
      axgl::ComponentContext& context,
      const std::vector<std::string>& tokens
    ) = 0;
  };

  class Commands
  {
  private:
    std::unordered_map<std::string, std::shared_ptr<Command>> commands_;

  public:
    void put_command(const std::string& name, std::shared_ptr<Command> command)
    {
      commands_.insert({ name, std::move(command) });
    }

    void call(
      axgl::ComponentContext& context,
      const std::string& name,
      const std::vector<std::string>& tokens
    )
    {
      if (commands_.contains(name))
        commands_.at(name)->run(context, tokens);
    }
  };

}
