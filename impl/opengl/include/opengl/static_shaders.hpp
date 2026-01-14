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
  [[nodiscard]] ShaderProgram& mesh_3d() const { return *mesh_3d_; }
  [[nodiscard]] ShaderProgram& text() const { return *text_; }
  [[nodiscard]] ShaderProgram& screen() const { return *screen_; }
  [[nodiscard]] ShaderProgram& weighted_blended() const { return *weighted_blended_; }
  [[nodiscard]] ShaderProgram& gui() const { return *gui_; }
  [[nodiscard]] ShaderProgram& color() const { return *color_; }

private:
  std::unique_ptr<ShaderProgram> mesh_2d_;
  std::unique_ptr<ShaderProgram> mesh_3d_;
  std::unique_ptr<ShaderProgram> text_;
  std::unique_ptr<ShaderProgram> screen_;
  std::unique_ptr<ShaderProgram> weighted_blended_;
  std::unique_ptr<ShaderProgram> gui_;
  std::unique_ptr<ShaderProgram> color_;

  StaticShaders()
  {
    mesh_2d_ = std::make_unique<ShaderProgram>(std::vector<ShaderProgram::Shader>{
      {GL_VERTEX_SHADER, axgl_opengl_impl_res::get("shader/mesh2d.vs")},
      {GL_FRAGMENT_SHADER, axgl_opengl_impl_res::get("shader/mesh2d.fs")}});
    mesh_3d_ = std::make_unique<ShaderProgram>(std::vector<ShaderProgram::Shader>{
      {GL_VERTEX_SHADER, axgl_opengl_impl_res::get("shader/mesh3d.vs")},
      {GL_FRAGMENT_SHADER, axgl_opengl_impl_res::get("shader/phong.fs")}});
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
  }
};

} // namespace opengl
