#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/context.hpp>
#include <axgl/interface/window.hpp>

namespace axgl
{

class Renderer : virtual public axgl::ContextHolder
{
public:
  virtual void render() = 0;

  // window
  virtual void set_window(axgl::ptr_t<axgl::Window> window) = 0;
  [[nodiscard]] virtual axgl::ptr_t<axgl::Window> get_window() const = 0;

  // msaa
  virtual void set_enable_msaa(bool enable_msaa) = 0;
  virtual void set_msaa_sample_count(std::uint32_t msaa_sample_count) = 0;
  [[nodiscard]] virtual bool get_enable_msaa() const = 0;
  [[nodiscard]] virtual std::uint32_t get_msaa_sample_count() const = 0;

  // shadow
  virtual void set_enable_shadow(bool enable_shadow) = 0;
  virtual void set_shadow_map_size(std::uint32_t shadow_map_size) = 0;
  virtual void set_shadow_distance(float shadow_distance) = 0;
  [[nodiscard]] virtual bool get_enable_shadow() const = 0;
  [[nodiscard]] virtual std::uint32_t get_shadow_map_size() const = 0;
  [[nodiscard]] virtual float get_shadow_distance() const = 0;

  // HDR
  virtual void set_enable_hdr(bool enable_hdr) = 0;
  virtual void set_exposure(float exposure) = 0;
  [[nodiscard]] virtual bool get_enable_hdr() const = 0;
  [[nodiscard]] virtual float get_exposure() const = 0;
};

} // namespace axgl
