#pragma once

#include <format>
#include <memory>
#include <string>
#include <vector>

#include <opengl/shader_program.hpp>

#include <axgl/impl/opengl/renderer/constants.hpp>
#include <axgl/impl/opengl/renderer/ssao.hpp>
#include <axgl/impl/opengl/renderer/sun_shadow_map.hpp>
#include <axgl_opengl_impl/res.hpp>

namespace axgl::impl::opengl
{

class Shaders
{
  std::unique_ptr<const ::opengl::ShaderProgram> mesh2d_;
  std::unique_ptr<const ::opengl::ShaderProgram> mesh3d_;
  std::unique_ptr<const ::opengl::ShaderProgram> mesh3d_opaque_;
  std::unique_ptr<const ::opengl::ShaderProgram> text_;
  std::unique_ptr<const ::opengl::ShaderProgram> screen_;
  std::unique_ptr<const ::opengl::ShaderProgram> weighted_blended_;
  std::unique_ptr<const ::opengl::ShaderProgram> gui_;
  std::unique_ptr<const ::opengl::ShaderProgram> color_;
  std::unique_ptr<const ::opengl::ShaderProgram> depth_only_;
  std::unique_ptr<const ::opengl::ShaderProgram> point_depth_;
  std::unique_ptr<const ::opengl::ShaderProgram> ssao_geometry_;
  std::unique_ptr<const ::opengl::ShaderProgram> ssao_;
  std::unique_ptr<const ::opengl::ShaderProgram> ssao_blur_;
  std::unique_ptr<const ::opengl::ShaderProgram> bloom_bright_pass_;
  std::unique_ptr<const ::opengl::ShaderProgram> bloom_blur_;
  std::unique_ptr<const ::opengl::ShaderProgram> bloom_composite_;

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

  [[nodiscard]] const auto& mesh2d() const { return *mesh2d_; }
  [[nodiscard]] const auto& mesh3d() const { return *mesh3d_; }
  [[nodiscard]] const auto& mesh3d_opaque() const { return *mesh3d_opaque_; }
  [[nodiscard]] const auto& text() const { return *text_; }
  [[nodiscard]] const auto& screen() const { return *screen_; }
  [[nodiscard]] const auto& blend() const { return *weighted_blended_; }
  [[nodiscard]] const auto& gui() const { return *gui_; }
  [[nodiscard]] const auto& color() const { return *color_; }
  [[nodiscard]] const auto& depth_only() const { return *depth_only_; }
  [[nodiscard]] const auto& point_depth() const { return *point_depth_; }
  [[nodiscard]] const auto& ssao_geometry() const { return *ssao_geometry_; }
  [[nodiscard]] const auto& ssao() const { return *ssao_; }
  [[nodiscard]] const auto& ssao_blur() const { return *ssao_blur_; }
  [[nodiscard]] const auto& bloom_bright_pass() const
  {
    return *bloom_bright_pass_;
  }
  [[nodiscard]] const auto& bloom_blur() const { return *bloom_blur_; }
  [[nodiscard]] const auto& bloom_composite() const
  {
    return *bloom_composite_;
  }

private:
  Shaders()
  {
    using namespace axgl_opengl_impl_res;

    mesh2d_ = std::make_unique<::opengl::ShaderProgram>(
      std::vector<::opengl::ShaderProgram::Shader>{
        {GL_VERTEX_SHADER, get("shader/mesh2d.vs")},
        {GL_FRAGMENT_SHADER, get("shader/mesh2d.fs")}
      }
    );
    // mesh3d
    {
      const std::string defines = std::format(
        "#define SUN_LIGHT_LIMIT {}\n"
        "#define SUN_SHADOW_LIMIT {}\n"
        "#define SUN_SHADOW_CASCADE_COUNT {}\n"
        "#define SPOT_LIGHT_LIMIT {}\n"
        "#define SPOT_SHADOW_LIMIT {}\n"
        "#define POINT_LIGHT_LIMIT {}\n"
        "#define POINT_SHADOW_LIMIT {}\n",
        renderer::kSunLightLimit,         //
        renderer::kSunShadowLimit,        //
        renderer::kSunShadowCascadeCount, //
        renderer::kSpotLightLimit,        //
        renderer::kSpotShadowLimit,       //
        renderer::kPointLightLimit,       //
        renderer::kPointShadowLimit       //
      );
      const auto mesh3d_vs = with_defines(get("shader/mesh3d.vs"), defines);
      const auto mesh3d_fs = with_defines(get("shader/mesh3d.fs"), defines);
      mesh3d_ = std::make_unique<::opengl::ShaderProgram>(
        std::vector<::opengl::ShaderProgram::Shader>{
          {GL_VERTEX_SHADER, mesh3d_vs}, {GL_FRAGMENT_SHADER, mesh3d_fs}
        }
      );
      // opaque variant: same sources with OPAQUE_PASS defined, dropping the
      // MRT reveal output and OIT weighting so opaque draws only need a
      // single-attachment framebuffer.
      const std::string opaque_fs = with_defines(
        get("shader/mesh3d.fs"), defines + "#define OPAQUE_PASS\n"
      );
      mesh3d_opaque_ = std::make_unique<::opengl::ShaderProgram>(
        std::vector<::opengl::ShaderProgram::Shader>{
          {GL_VERTEX_SHADER, mesh3d_vs}, {GL_FRAGMENT_SHADER, opaque_fs}
        }
      );
    }
    text_ = std::make_unique<::opengl::ShaderProgram>(
      std::vector<::opengl::ShaderProgram::Shader>{
        {GL_VERTEX_SHADER, get("shader/mesh2d.vs")},
        {GL_FRAGMENT_SHADER, get("shader/text.fs")}
      }
    );
    screen_ = std::make_unique<::opengl::ShaderProgram>(
      std::vector<::opengl::ShaderProgram::Shader>{
        {GL_VERTEX_SHADER, get("shader/screen.vs")},
        {GL_FRAGMENT_SHADER, get("shader/screen.fs")}
      }
    );
    weighted_blended_ = std::make_unique<::opengl::ShaderProgram>(
      std::vector<::opengl::ShaderProgram::Shader>{
        {GL_VERTEX_SHADER, get("shader/screen.vs")},
        {GL_FRAGMENT_SHADER, get("shader/weighted_blended.fs")}
      }
    );
    gui_ = std::make_unique<::opengl::ShaderProgram>(
      std::vector<::opengl::ShaderProgram::Shader>{
        {GL_VERTEX_SHADER, get("shader/gui.vs")},
        {GL_FRAGMENT_SHADER, get("shader/gui.fs")}
      }
    );
    color_ = std::make_unique<::opengl::ShaderProgram>(
      std::vector<::opengl::ShaderProgram::Shader>{
        {GL_VERTEX_SHADER, get("shader/color.vs")},
        {GL_FRAGMENT_SHADER, get("shader/color.fs")}
      }
    );
    depth_only_ = std::make_unique<::opengl::ShaderProgram>(
      std::vector<::opengl::ShaderProgram::Shader>{
        {GL_VERTEX_SHADER, get("shader/depth_only.vs")},
        {GL_FRAGMENT_SHADER, get("shader/depth_only.fs")}
      }
    );
    point_depth_ = std::make_unique<::opengl::ShaderProgram>(
      std::vector<::opengl::ShaderProgram::Shader>{
        {GL_VERTEX_SHADER, get("shader/point_depth.vs")},
        {GL_FRAGMENT_SHADER, get("shader/point_depth.fs")}
      }
    );
    ssao_geometry_ = std::make_unique<::opengl::ShaderProgram>(
      std::vector<::opengl::ShaderProgram::Shader>{
        {GL_VERTEX_SHADER, get("shader/ssao_geometry.vs")},
        {GL_FRAGMENT_SHADER, get("shader/ssao_geometry.fs")}
      }
    );
    {
      const std::string defines
        = std::format("#define KERNEL_SIZE {}\n", renderer::kSsaoKernelSize);
      const auto ssao_fs = with_defines(get("shader/ssao.fs"), defines);
      ssao_ = std::make_unique<::opengl::ShaderProgram>(
        std::vector<::opengl::ShaderProgram::Shader>{
          {GL_VERTEX_SHADER, get("shader/screen.vs")},
          {GL_FRAGMENT_SHADER, ssao_fs}
        }
      );
    }
    ssao_blur_ = std::make_unique<::opengl::ShaderProgram>(
      std::vector<::opengl::ShaderProgram::Shader>{
        {GL_VERTEX_SHADER, get("shader/screen.vs")},
        {GL_FRAGMENT_SHADER, get("shader/ssao_blur.fs")}
      }
    );
    bloom_bright_pass_ = std::make_unique<::opengl::ShaderProgram>(
      std::vector<::opengl::ShaderProgram::Shader>{
        {GL_VERTEX_SHADER, get("shader/screen.vs")},
        {GL_FRAGMENT_SHADER, get("shader/bloom_bright_pass.fs")}
      }
    );
    bloom_blur_ = std::make_unique<::opengl::ShaderProgram>(
      std::vector<::opengl::ShaderProgram::Shader>{
        {GL_VERTEX_SHADER, get("shader/screen.vs")},
        {GL_FRAGMENT_SHADER, get("shader/bloom_blur.fs")}
      }
    );
    bloom_composite_ = std::make_unique<::opengl::ShaderProgram>(
      std::vector<::opengl::ShaderProgram::Shader>{
        {GL_VERTEX_SHADER, get("shader/screen.vs")},
        {GL_FRAGMENT_SHADER, get("shader/bloom_composite.fs")}
      }
    );
  }

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
#ifdef AXGL_DEBUG
    out += "\n#define AXGL_DEBUG\n";
#endif
    out += defines;
    out += str.substr(nl + 1);
    return out;
  }
};

} // namespace axgl::impl::opengl
