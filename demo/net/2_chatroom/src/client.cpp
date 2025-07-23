#include <ftxui/component/component.hpp>      // for Input, Renderer, Button, Container
#include <ftxui/component/component_base.hpp> // for ComponentBase
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>             // for Element, vbox, text, separator, hbox

#include <deque>
#include <string>

using namespace ftxui;

int main() {
  // A deque to store chat messages
  std::deque<std::string> chat_messages;

  // Input box for typing messages
  std::string input_text;
  auto input = Input(&input_text, "Type your message...");

  // Button to send messages
  auto send_button = Button("Send", [&] {
    if (!input_text.empty()) {
      chat_messages.push_back(input_text); // Add the message to the chat
      input_text.clear();                 // Clear the input box
    }
  });

  // Renderer for the chat messages
  auto chat_renderer = Renderer([&] {
    // Display messages as a vertical list of text elements
    std::vector<Element> message_elements;
    for (const auto& message : chat_messages) {
      message_elements.push_back(text("-> " + message));
    }

    // Create a scrollable message box
    return vbox(message_elements) | vscroll_indicator | frame | border;
  });

  // Combine input and button into a single input box
  auto input_box = Container::Horizontal({
      input,
      send_button,
  });

  // Main container for the layout
  auto main_container = Container::Vertical({
      chat_renderer,
      input_box,
  });

  // Renderer for the main layout
  auto layout = Renderer(main_container, [&] {
    return vbox({
        chat_renderer->Render() | flex, // Chat messages take up most of the space
        separator(),
        hbox({
            input->Render() | flex, // Input box stretches horizontally
            send_button->Render() | size(WIDTH, GREATER_THAN, 8),
        }) | border,
    });
  });

  // Run the application
  auto screen = ScreenInteractive::Fullscreen();
  screen.Loop(main_container);

  return 0;
}