#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/input.hpp>
#include <axgl/interface/services/input_service.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/opengl/renderer.hpp>
#include <axgl/impl/service_base.hpp>

class InputManager : public axgl::impl::ServiceBase
{
public:
  static constexpr std::string_view kTypeId = "service::input-manager";

private:
  axgl::ptr_t<axgl::Input> rotate_sun_;
  axgl::ptr_t<axgl::Input> flashlight_;
  axgl::ptr_t<axgl::Input> switch_scene_;
  axgl::ptr_t<axgl::Input> blend_;
  axgl::ptr_t<axgl::Input> msaa_;
  axgl::ptr_t<axgl::Input> debug_;
  axgl::ptr_t<axgl::Input> shadow_;
  axgl::ptr_t<axgl::Input> debug_csm_;
  axgl::ptr_t<axgl::Input> hdr_;
  axgl::ptr_t<axgl::Input> exposure_up_;
  axgl::ptr_t<axgl::Input> exposure_down_;
  axgl::ptr_t<axgl::Input> ssao_;
  axgl::ptr_t<axgl::Input> bloom_;

public:
  [[nodiscard]] auto rotate_sun() const { return rotate_sun_; }
  [[nodiscard]] auto flashlight() const { return flashlight_; }
  [[nodiscard]] auto switch_scene() const { return switch_scene_; }
  [[nodiscard]] auto blend() const { return blend_; }
  [[nodiscard]] auto msaa() const { return msaa_; }
  [[nodiscard]] auto debug() const { return debug_; }
  [[nodiscard]] auto shadow() const { return shadow_; }
  [[nodiscard]] auto debug_csm() const { return debug_csm_; }
  [[nodiscard]] auto hdr() const { return hdr_; }
  [[nodiscard]] auto exposure_up() const { return exposure_up_; }
  [[nodiscard]] auto exposure_down() const { return exposure_down_; }
  [[nodiscard]] auto ssao() const { return ssao_; }
  [[nodiscard]] auto bloom() const { return bloom_; }

  void initialize() override
  {
    rotate_sun_
      = axgl::create_ptr<axgl::Input>("Rotate Sun", axgl::Input::Source::kKeyR);
    exposure_up_ = axgl::create_ptr<axgl::Input>(
      "Exposure Up", axgl::Input::Source::kKeyUp
    );
    exposure_down_ = axgl::create_ptr<axgl::Input>(
      "Exposure Down", axgl::Input::Source::kKeyDown
    );
    flashlight_ = axgl::create_ptr<axgl::Input>(
      "Toggle Flashlight", axgl::Input::Source::kKeyF
    );
    switch_scene_ = axgl::create_ptr<axgl::Input>(
      "Switch Scene", axgl::Input::Source::kKeyF1
    );
    blend_ = axgl::create_ptr<axgl::Input>(
      "Toggle Blend", axgl::Input::Source::kKeyF2
    );
    msaa_ = axgl::create_ptr<axgl::Input>(
      "Toggle MSAA", axgl::Input::Source::kKeyF3
    );
    debug_ = axgl::create_ptr<axgl::Input>(
      "Toggle Debug Mode", axgl::Input::Source::kKeyF4
    );
    shadow_ = axgl::create_ptr<axgl::Input>(
      "Toggle Shadow", axgl::Input::Source::kKeyF5
    );
    debug_csm_ = axgl::create_ptr<axgl::Input>(
      "Toggle CSM Borders", axgl::Input::Source::kKeyF6
    );
    hdr_ = axgl::create_ptr<axgl::Input>(
      "Toggle HDR", axgl::Input::Source::kKeyF7
    );
    ssao_ = axgl::create_ptr<axgl::Input>(
      "Toggle SSAO", axgl::Input::Source::kKeyF8
    );
    bloom_ = axgl::create_ptr<axgl::Input>(
      "Toggle Bloom", axgl::Input::Source::kKeyF9
    );
  }

  void on_start() override
  {
    const auto& input_service = axgl_->input_service();
    input_service->add_input(rotate_sun_);
    input_service->add_input(flashlight_);
    input_service->add_input(switch_scene_);
    input_service->add_input(blend_);
    input_service->add_input(msaa_);
    input_service->add_input(debug_);
    input_service->add_input(shadow_);
    input_service->add_input(debug_csm_);
    input_service->add_input(hdr_);
    input_service->add_input(exposure_up_);
    input_service->add_input(exposure_down_);
    input_service->add_input(ssao_);
    input_service->add_input(bloom_);
  }
};
