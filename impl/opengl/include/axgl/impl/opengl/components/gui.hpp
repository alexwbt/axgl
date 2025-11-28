#pragma once

#include <axgl/interface/components/gui.hpp>

#include <axgl/impl/component_base.hpp>

namespace axgl::impl::opengl::component
{

class Gui : public virtual axgl::component::Gui, public axgl::impl::ComponentBase
{
  std::shared_ptr<axgl::gui::Element> root_;

public:
  std::shared_ptr<axgl::gui::Element> root() override { return root_; }
  void set_root(std::shared_ptr<axgl::gui::Element> root) override { root_ = std::move(root); }
};

} // namespace axgl::impl::opengl::component
