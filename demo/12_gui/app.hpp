#pragma once

#include <string>
#include <utility>

#include <axgl/axgl.hpp>
#include <axgl/common.hpp>
#include <axgl/interface/gui/page.hpp>
#include <axgl/interface/renderer.hpp>
#include <axgl/interface/services/gui_service.hpp>
#include <axgl/interface/window.hpp>

#include "setup_input.hpp"
#include "ui.hpp"

namespace demo_gui {

struct App {
  axgl::Axgl& engine;
  Inputs inputs;
  Ui ui;

  App(axgl::Axgl& engine, Inputs inputs) :
    engine(engine), inputs(std::move(inputs)), ui{engine.gui_service()} {}

  [[nodiscard]] axgl::ptr_t<axgl::GuiService> gui() const {
    return engine.gui_service();
  }

  [[nodiscard]] axgl::ptr_t<axgl::Renderer> renderer() const {
    return engine.renderer_service()->get_active_renderer();
  }

  [[nodiscard]] axgl::ptr_t<axgl::Window> window() const {
    return renderer()->get_window();
  }

  axgl::ptr_t<axgl::gui::Page> create_page(const std::string& path) {
    const auto page = gui()->create_page();
    inputs.apply(page);
    gui()->register_page(path, page);
    return page;
  }

  void navigate(const std::string& path) { gui()->navigate(path); }
};

} // namespace demo_gui
