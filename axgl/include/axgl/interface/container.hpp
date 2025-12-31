#pragma once

#include <axgl/common.hpp>

namespace axgl
{

class Component;

template <typename Item>
class Container
{
public:
  virtual ~Container() = default;

  virtual void add(axgl::ptr_t<Item> item) = 0;

  virtual void add_all(const std::span<const axgl::ptr_t<Item>>& item)
  {
    for (auto& item_ : item)
      add(item_);
  }

  virtual void remove(const axgl::ptr_t<Item>& item) = 0;

  // TODO: find a better way
  [[nodiscard]] virtual std::span<const axgl::ptr_t<Item>> get() const = 0;

  template <typename ItemType>
  axgl::ptr_t<ItemType> get_t()
  {
    for (const auto& item : get())
      if (const auto& item_t = axgl::ptr_cast<ItemType>(item)) return item_t;
    return nullptr;
  }
};

} // namespace axgl

