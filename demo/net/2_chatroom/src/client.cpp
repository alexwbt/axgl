#include <memory>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include "tui/chat.hpp"

int main()
{
  auto chat = std::make_shared<tui::Chat>();

  const auto renderer = ftxui::Renderer(chat, [&] { return chat->Render(); });

  auto screen = ftxui::ScreenInteractive::Fullscreen();
  screen.Loop(renderer);
}
