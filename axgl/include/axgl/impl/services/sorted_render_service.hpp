#pragma once

#include <functional>
#include <ranges>
#include <string_view>
#include <utility>
#include <vector>

#include <axgl/interface/service.hpp>

namespace axgl::impl
{

class SortedRenderService : public Service
{
public:
  static constexpr std::string_view kTypeId = "service::sorted_render";

private:
  std::vector<std::pair<float, std::function<void()>>> sorted_renders_;

public:
  void add_sorted_render(float key, std::function<void()> render_func)
  {
    ZoneScopedN("Add Sorted Render");
    sorted_renders_.emplace_back(key, std::move(render_func));
  }

  void render()
  {
    if (sorted_renders_.empty())
      return;

    ZoneScopedN("Sorted Render");

    // sort sorted_renders_ by key descending
    std::ranges::sort(
      sorted_renders_, [](const auto& a, const auto& b)
    {
      return a.first > b.first;
    });

    // render
    for (const auto& render_func : sorted_renders_ | std::views::values)
      render_func();

    // clear list
    sorted_renders_.clear();
  }
};

} // namespace axgl::impl
