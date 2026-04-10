#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/input.hpp>
#include <axgl/interface/services/input_service.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/service_base.hpp>

class InputManager : public axgl::impl::ServiceBase
{
public:
  static constexpr std::string_view kTypeId = "service::input-manager";

private:
  axgl::ptr_t<axgl::Input> rotate_sun_;
  axgl::ptr_t<axgl::Input> flashlight_;
  axgl::ptr_t<axgl::Input> debug_;
  axgl::ptr_t<axgl::Input> msaa_;

public:
  [[nodiscard]] auto rotate_sun() const { return rotate_sun_; }
  [[nodiscard]] auto flashlight() const { return flashlight_; }
  [[nodiscard]] auto debug() const { return debug_; }
  [[nodiscard]] auto msaa() const { return msaa_; }

  void initialize() override
  {
    rotate_sun_ = axgl::create_ptr<axgl::Input>("Rotate Sun", axgl::Input::Source::kKeyR);
    flashlight_ = axgl::create_ptr<axgl::Input>("Toggle Flashlight", axgl::Input::Source::kKeyF);
    debug_ = axgl::create_ptr<axgl::Input>("Toggle Debug Mode", axgl::Input::Source::kKeyF3);
    msaa_ = axgl::create_ptr<axgl::Input>("Toggle MSAA", axgl::Input::Source::kKeyM);
  }

  void on_start() override
  {
    const auto& input_service = axgl_->input_service();
    input_service->add_input(rotate_sun_);
    input_service->add_input(flashlight_);
    input_service->add_input(debug_);
    input_service->add_input(msaa_);
  }

  void update() override
  {
    if (msaa_->tick == 1)
    {
      const auto& renderer = axgl_->renderer_service()->get_active_renderer();
      renderer->set_antialiasing(!renderer->get_antialiasing());
    }
  }
};
