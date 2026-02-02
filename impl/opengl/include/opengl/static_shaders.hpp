#pragma once

#include <memory>
#include <vector>

#include <opengl/shader_program.hpp>

#include <axgl_opengl_impl/res.hpp>

namespace opengl
{

class StaticShaders
{
public:
  static const StaticShaders& instance()
  {
    static StaticShaders instance_;
    return instance_;
  }

  StaticShaders(const StaticShaders&) = delete;
  StaticShaders& operator=(const StaticShaders&) = delete;
  StaticShaders(const StaticShaders&&) = delete;
  StaticShaders& operator=(const StaticShaders&&) = delete;

  [[nodiscard]] ShaderProgram& mesh_2d() const { return *mesh_2d_; }
  [[nodiscard]] ShaderProgram& blinn_phong() const { return *blinn_phong_; }
  [[nodiscard]] ShaderProgram& text() const { return *text_; }
  [[nodiscard]] ShaderProgram& screen() const { return *screen_; }
  [[nodiscard]] ShaderProgram& weighted_blended() const { return *weighted_blended_; }
  [[nodiscard]] ShaderProgram& gui() const { return *gui_; }
  [[nodiscard]] ShaderProgram& color() const { return *color_; }
  [[nodiscard]] ShaderProgram& depth_only() const { return *depth_only_; }

private:
  std::unique_ptr<ShaderProgram> mesh_2d_;
  std::unique_ptr<ShaderProgram> blinn_phong_;
  std::unique_ptr<ShaderProgram> text_;
  std::unique_ptr<ShaderProgram> screen_;
  std::unique_ptr<ShaderProgram> weighted_blended_;
  std::unique_ptr<ShaderProgram> gui_;
  std::unique_ptr<ShaderProgram> color_;
  std::unique_ptr<ShaderProgram> depth_only_;

  StaticShaders()
  {
    mesh_2d_ = std::make_unique<ShaderProgram>(std::vector<ShaderProgram::Shader>{
      {GL_VERTEX_SHADER, axgl_opengl_impl_res::get("shader/mesh2d.vs")},
      {GL_FRAGMENT_SHADER, axgl_opengl_impl_res::get("shader/mesh2d.fs")}});
    blinn_phong_ = std::make_unique<ShaderProgram>(std::vector<ShaderProgram::Shader>{
      {GL_VERTEX_SHADER, axgl_opengl_impl_res::get("shader/mesh3d.vs")},
      {GL_FRAGMENT_SHADER, axgl_opengl_impl_res::get("shader/blinn_phong.fs")}});
    text_ = std::make_unique<ShaderProgram>(std::vector<ShaderProgram::Shader>{
      {GL_VERTEX_SHADER, axgl_opengl_impl_res::get("shader/mesh2d.vs")},
      {GL_FRAGMENT_SHADER, axgl_opengl_impl_res::get("shader/text.fs")}});
    screen_ = std::make_unique<ShaderProgram>(std::vector<ShaderProgram::Shader>{
      {GL_VERTEX_SHADER, axgl_opengl_impl_res::get("shader/screen.vs")},
      {GL_FRAGMENT_SHADER, axgl_opengl_impl_res::get("shader/screen.fs")}});
    weighted_blended_ = std::make_unique<ShaderProgram>(std::vector<ShaderProgram::Shader>{
      {GL_VERTEX_SHADER, axgl_opengl_impl_res::get("shader/screen.vs")},
      {GL_FRAGMENT_SHADER, axgl_opengl_impl_res::get("shader/weighted_blended.fs")}});
    gui_ = std::make_unique<ShaderProgram>(std::vector<ShaderProgram::Shader>{
      {GL_VERTEX_SHADER, axgl_opengl_impl_res::get("shader/gui.vs")},
      {GL_FRAGMENT_SHADER, axgl_opengl_impl_res::get("shader/gui.fs")}});
    color_ = std::make_unique<ShaderProgram>(std::vector<ShaderProgram::Shader>{
      {GL_VERTEX_SHADER, axgl_opengl_impl_res::get("shader/mesh3d.vs")},
      {GL_FRAGMENT_SHADER, axgl_opengl_impl_res::get("shader/color.fs")}});
    depth_only_ = std::make_unique<ShaderProgram>(std::vector<ShaderProgram::Shader>{
      {GL_VERTEX_SHADER, axgl_opengl_impl_res::get("shader/depth_only.vs")},
      {GL_FRAGMENT_SHADER, axgl_opengl_impl_res::get("shader/depth_only.fs")}});
  }
};

} // namespace opengl
