#include <format>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

int main()
{
  using namespace ftxui;

  std::string input_value;
  std::vector<std::string> message_values;

  auto input = Input(&input_value);
  input |= CatchEvent([&](const Event& event)
  {
    if (event == Event::Return)
    {
      message_values.push_back(input_value);
      input_value.clear();
      return true;
    }
    return false;
  });

  const auto renderer = Renderer(
    input, [&]
  {
    std::vector<Element> message_elements;
    message_elements.reserve(message_values.size());
    for (const auto& value : message_values)
      message_elements.push_back(paragraph(std::format("> {}", value)));

    return vbox({
      vbox(std::move(message_elements)),
      input->Render(),
    });
  });

  auto screen = ScreenInteractive::Fullscreen();
  screen.Loop(renderer);
}
