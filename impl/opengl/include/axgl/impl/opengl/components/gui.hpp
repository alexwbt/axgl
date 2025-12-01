#pragma once

#include <axgl/interface/components/gui.hpp>

#include <axgl/impl/component_base.hpp>

namespace axgl::impl::opengl::component
{

class Gui : public virtual axgl::component::Gui, public axgl::impl::ComponentBase
{
  axgl::ptr_t<axgl::gui::Element> root_;

public:
  axgl::ptr_t<axgl::gui::Element> root() override { return root_; }
  void set_root(axgl::ptr_t<axgl::gui::Element> root) override { root_ = std::move(root); }
};

} // namespace axgl::impl::opengl::component
