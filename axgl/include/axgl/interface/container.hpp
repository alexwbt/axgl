#pragma once

#include <memory>
#include <vector>

#include <axgl/util/iterable.hpp>

namespace axgl
{

class Component;

template <typename Item>
class Container
{
public:
  virtual ~Container() = default;

  virtual void add(std::shared_ptr<Item> item) = 0;

  virtual void add_all(const std::vector<std::shared_ptr<Item>>& item)
  {
    for (auto& item_ : item)
      add(item_);
  }

  virtual void remove(const std::shared_ptr<Item>& item) = 0;

  [[nodiscard]] virtual util::Iterable<std::shared_ptr<Item>> get() const = 0;

  template <typename ItemType>
  std::shared_ptr<ItemType> get_t()
  {
    for (const auto& item : get())
      if (auto item_t = std::dynamic_pointer_cast<ItemType>(item))
        return item_t;
    return nullptr;
  }
};

} // namespace axgl

