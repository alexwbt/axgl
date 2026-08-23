#pragma once

#include <memory>
#include <string>
#include <vector>

#include <opengl/shader_program.hpp>

#include <axgl/impl/opengl/renderer/constants.hpp>
#include <axgl/impl/opengl/renderer/shadow_map.hpp>
#include <axgl_opengl_impl/res.hpp>

namespace axgl::impl::opengl::renderer
{

class Shaders
{
public:
  static const Shaders& instance()
  {
    static Shaders instance_;
    return instance_;
  }

  Shaders(const Shaders&) = delete;
  Shaders& operator=(const Shaders&) = delete;
  Shaders(const Shaders&&) = delete;
  Shaders& operator=(const Shaders&&) = delete;

  [[nodiscard]] ::opengl::ShaderProgram& mesh2d() const { return *mesh2d_; }
  [[nodiscard]] ::opengl::ShaderProgram& mesh3d() const { return *mesh3d_; }
  [[nodiscard]] ::opengl::ShaderProgram& mesh3d_opaque() const
  {
    return *mesh3d_opaque_;
  }
  [[nodiscard]] ::opengl::ShaderProgram& text() const { return *text_; }
  [[nodiscard]] ::opengl::ShaderProgram& screen() const { return *screen_; }
  [[nodiscard]] ::opengl::ShaderProgram& weighted_blended() const
  {
    return *weighted_blended_;
  }
  [[nodiscard]] ::opengl::ShaderProgram& gui() const { return *gui_; }
  [[nodiscard]] ::opengl::ShaderProgram& color() const { return *color_; }
  [[nodiscard]] ::opengl::ShaderProgram& depth_only() const
  {
    return *depth_only_;
  }

private:
  std::unique_ptr<::opengl::ShaderProgram> mesh2d_;
  std::unique_ptr<::opengl::ShaderProgram> mesh3d_;
  std::unique_ptr<::opengl::ShaderProgram> mesh3d_opaque_;
  std::unique_ptr<::opengl::ShaderProgram> text_;
  std::unique_ptr<::opengl::ShaderProgram> screen_;
  std::unique_ptr<::opengl::ShaderProgram> weighted_blended_;
  std::unique_ptr<::opengl::ShaderProgram> gui_;
  std::unique_ptr<::opengl::ShaderProgram> color_;
  std::unique_ptr<::opengl::ShaderProgram> depth_only_;

  // injects `defines` after the #version line (which GLSL requires to be the
  // first statement), then appends the rest of `source`.
  static std::string with_defines(
    const std::span<const std::uint8_t>& source, const std::string& defines
  )
  {
    const std::string str(
      reinterpret_cast<const char*>(source.data()), source.size()
    );
    const auto nl = str.find('\n');
    std::string out = str.substr(0, nl + 1);
    out += defines;
    out += str.substr(nl + 1);
    return out;
  }

  // defines shared by all shaders that include mesh3d.vs or mesh3d.fs, so the
  // light array and cascade array sizes stay in sync with the C++ constants.
  static std::string mesh3d_defines()
  {
    return std::string("#define LIGHT_COUNT ") + std::to_string(kLightCount)
      + "\n#define CASCADE_COUNT " + std::to_string(kCascadeCount) + "\n";
  }

  Shaders()
  {
    const std::string defines = mesh3d_defines();
    const auto mesh3d_vs
      = with_defines(axgl_opengl_impl_res::get("shader/mesh3d.vs"), defines);
    const auto mesh3d_fs
      = with_defines(axgl_opengl_impl_res::get("shader/mesh3d.fs"), defines);

    mesh2d_ = std::make_unique<::opengl::ShaderProgram>(
      std::vector<::opengl::ShaderProgram::Shader>{
        {GL_VERTEX_SHADER, axgl_opengl_impl_res::get("shader/mesh2d.vs")},
        {GL_FRAGMENT_SHADER, axgl_opengl_impl_res::get("shader/mesh2d.fs")}
      }
    );
    mesh3d_ = std::make_unique<::opengl::ShaderProgram>(
      std::vector<::opengl::ShaderProgram::Shader>{
        {GL_VERTEX_SHADER, mesh3d_vs}, {GL_FRAGMENT_SHADER, mesh3d_fs}
      }
    );
    {
      // opaque variant: same sources with OPAQUE_PASS defined, dropping the
      // MRT reveal output and OIT weighting so opaque draws only need a
      // single-attachment framebuffer.
      const std::string opaque_fs = with_defines(
        axgl_opengl_impl_res::get("shader/mesh3d.fs"),
        defines + "#define OPAQUE_PASS\n"
      );
      mesh3d_opaque_ = std::make_unique<::opengl::ShaderProgram>(
        std::vector<::opengl::ShaderProgram::Shader>{
          {GL_VERTEX_SHADER, mesh3d_vs}, {GL_FRAGMENT_SHADER, opaque_fs}
        }
      );
    }
    text_ = std::make_unique<::opengl::ShaderProgram>(
      std::vector<::opengl::ShaderProgram::Shader>{
        {GL_VERTEX_SHADER, axgl_opengl_impl_res::get("shader/mesh2d.vs")},
        {GL_FRAGMENT_SHADER, axgl_opengl_impl_res::get("shader/text.fs")}
      }
    );
    screen_ = std::make_unique<::opengl::ShaderProgram>(
      std::vector<::opengl::ShaderProgram::Shader>{
        {GL_VERTEX_SHADER, axgl_opengl_impl_res::get("shader/screen.vs")},
        {GL_FRAGMENT_SHADER, axgl_opengl_impl_res::get("shader/screen.fs")}
      }
    );
    weighted_blended_ = std::make_unique<::opengl::ShaderProgram>(
      std::vector<::opengl::ShaderProgram::Shader>{
        {GL_VERTEX_SHADER, axgl_opengl_impl_res::get("shader/screen.vs")},
        {GL_FRAGMENT_SHADER,
         axgl_opengl_impl_res::get("shader/weighted_blended.fs")}
      }
    );
    gui_ = std::make_unique<::opengl::ShaderProgram>(
      std::vector<::opengl::ShaderProgram::Shader>{
        {GL_VERTEX_SHADER, axgl_opengl_impl_res::get("shader/gui.vs")},
        {GL_FRAGMENT_SHADER, axgl_opengl_impl_res::get("shader/gui.fs")}
      }
    );
    color_ = std::make_unique<::opengl::ShaderProgram>(
      std::vector<::opengl::ShaderProgram::Shader>{
        {GL_VERTEX_SHADER, mesh3d_vs},
        {GL_FRAGMENT_SHADER, axgl_opengl_impl_res::get("shader/color.fs")}
      }
    );
    depth_only_ = std::make_unique<::opengl::ShaderProgram>(
      std::vector<::opengl::ShaderProgram::Shader>{
        {GL_VERTEX_SHADER, axgl_opengl_impl_res::get("shader/depth_only.vs")},
        {GL_FRAGMENT_SHADER, axgl_opengl_impl_res::get("shader/depth_only.fs")}
      }
    );
  }
};

} // namespace axgl::impl::opengl::renderer
