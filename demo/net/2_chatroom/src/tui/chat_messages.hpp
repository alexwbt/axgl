#pragma once

#include <algorithm>
#include <cstdint>
#include <mutex>

#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>

namespace tui
{

class ChatMessages : public ftxui::ComponentBase
{
  std::vector<std::string> message_values_;
  std::int64_t focused_ = 0;

  // for mouse clicks
  std::vector<ftxui::Box> boxes_;

  std::mutex mutex_;

public:
  [[nodiscard]] std::int64_t size() const { return static_cast<std::int64_t>(message_values_.size()); }

  void set_focus(std::int64_t index)
  {
    std::lock_guard lock(mutex_);
    const auto& max = static_cast<std::int64_t>(message_values_.size() - 1);
    focused_ = std::max<std::int64_t>(0, std::min(max, index));
  }

  void add_message(const std::string& message)
  {
    std::lock_guard lock(mutex_);
    message_values_.push_back(message);
    if (focused_ == size() - 2) ++focused_;
    if (auto* screen = ftxui::ScreenInteractive::Active()) { screen->Post(ftxui::Event::Custom); }
  }

  void clear_messages()
  {
    std::lock_guard lock(mutex_);
    message_values_.clear();
    focused_ = 0;
    if (auto* screen = ftxui::ScreenInteractive::Active()) { screen->Post(ftxui::Event::Custom); }
  }

  [[nodiscard]] bool Focusable() const override { return true; }

  bool OnEvent(ftxui::Event event) override
  {
    using namespace ftxui;

    if (!CaptureMouse(event)) return false;

    if (event.is_mouse())
    {
      const auto& mouse = event.mouse();
      if (mouse.button == Mouse::WheelUp)
      {
        set_focus(focused_ - 1);
        return true;
      }
      if (mouse.button == Mouse::WheelDown)
      {
        set_focus(focused_ + 1);
        return true;
      }
      if (event.mouse().button == Mouse::Left)
      {
        for (std::int64_t i = 0; i < size(); ++i)
        {
          if (boxes_[i].Contain(event.mouse().x, event.mouse().y))
          {
            set_focus(i);
            TakeFocus();
            return true;
          }
        }
      }
      return false;
    }

    return false;
  }

  ftxui::Element OnRender() override
  {
    using namespace ftxui;

    std::lock_guard lock(mutex_);

    const auto& size = this->size();

    if (size <= 0) return vbox();

    boxes_.resize(size);

    Elements message_boxes_;

    for (std::int64_t i = 0; i < size; ++i)
    {
      const auto& message = message_values_.at(i);
      const auto& focused = focused_ == i;

      message_boxes_.push_back(
        hbox({text("> ") | (focused ? color(Color::LightSlateGrey) : nothing), paragraph(message)})
        | (focused ? focus : nothing) | reflect(boxes_[i]));
    }

    return vbox(message_boxes_);
  }
};

} // namespace tui
