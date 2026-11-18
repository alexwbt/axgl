#pragma once

#include <array>
#include <cstddef>
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

enum class Route : std::size_t {
  kMenu = 0,
  kInputSettings = 1,
  kDisplaySettings = 2,
};

struct App {
  axgl::Axgl& engine;
  Inputs inputs;
  Ui ui;
  std::array<axgl::ptr_t<axgl::gui::Page>, 3> pages;

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

  axgl::ptr_t<axgl::gui::Page> create_page() {
    const auto page = gui()->create_page();
    inputs.apply(page);
    return page;
  }

  void navigate(Route route) {
    gui()->set_main_ui(pages[static_cast<std::size_t>(route)]);
  }
};

} // namespace demo_gui
