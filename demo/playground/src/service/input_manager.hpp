#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/input.hpp>
#include <axgl/interface/service.hpp>
#include <axgl/interface/services/input_service.hpp>

#include <axgl/axgl.hpp>

class InputManager : public axgl::Service
{
  axgl::ptr_t<axgl::Input> flashlight_;
  axgl::ptr_t<axgl::Input> debug_;
  axgl::ptr_t<axgl::Input> msaa_;

public:
  [[nodiscard]] auto flashlight() const { return flashlight_; }
  [[nodiscard]] auto debug() const { return debug_; }
  [[nodiscard]] auto msaa() const { return msaa_; }

  void initialize(const axgl::Service::Context& context) override
  {
    flashlight_ = axgl::create_ptr<axgl::Input>("Toggle Flashlight", axgl::Input::Source::kKeyF);
    debug_ = axgl::create_ptr<axgl::Input>("Toggle Debug Mode", axgl::Input::Source::kKeyF3);
    msaa_ = axgl::create_ptr<axgl::Input>("Toggle MSAA", axgl::Input::Source::kKeyM);
  }

  void on_start(const axgl::Service::Context& context) override
  {
    const auto& input_service = context.axgl.input_service();
    input_service->add_input(flashlight_);
    input_service->add_input(debug_);
    input_service->add_input(msaa_);
  }

  void update(const Context& context) override
  {
    if (msaa_->tick == 1)
    {
      const auto& renderer = context.axgl.renderer_service()->get_active_renderer();
      renderer->set_antialiasing(!renderer->get_antialiasing());
    }
  }
};
