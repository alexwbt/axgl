// #pragma once
//
// #include <memory>
// #include <vector>
//
// #include <axgl/interface/container.hpp>
// #include <axgl/interface/gui/element.hpp>
//
// #include <axgl/util/iterable.hpp>
//
// namespace axgl::impl::gui
// {
//
// class ElementContainer : virtual public Container<axgl::gui::Element>
// {
//   std::weak_ptr<axgl::gui::Element> parent_;
//   std::vector<ptr_t<axgl::gui::Element>> elements_;
//
// public:
//   explicit ElementContainer(std::weak_ptr<axgl::gui::Element> parent) : parent_(std::move(parent)) { }
//
//   void tick() const
//   {
//     for (const auto& element : elements_)
//       element->tick();
//   }
//
//   void update() const
//   {
//     for (const auto& element : elements_)
//       element->update();
//   }
//
//   void render() const
//   {
//     for (const auto& element : elements_)
//       element->render();
//   }
//
//   void on_create() const
//   {
//     for (const auto& element : elements_)
//       element->on_create();
//   }
//
//   void on_remove() const
//   {
//     for (const auto& element : elements_)
//       element->on_remove();
//   }
//
//   void add(ptr_t<axgl::gui::Element> element) override
//   {
//     element->set_parent(parent_);
//     elements_.push_back(std::move(element));
//   }
//
//   void remove(const ptr_t<axgl::gui::Element>& element) override { std::erase(elements_, element); }
//
//   [[nodiscard]] util::Iterable<ptr_t<axgl::gui::Element>> get() const override
//   {
//     return util::to_iterable_t<ptr_t<axgl::gui::Element>>(elements_);
//   }
// };
//
// } // namespace axgl::impl::gui
