#pragma once

#include <array>
#include <memory>

#include <opengl/shader_program.hpp>

#include <axgl_opengl_impl/res.hpp>

namespace opengl
{

class StaticShaders
{
  std::unique_ptr<const ::opengl::ShaderProgram> text_;

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

  [[nodiscard]] const auto& text() const { return *text_; }

private:
  StaticShaders()
  {
    using namespace axgl_opengl_impl_res;

    text_ = std::make_unique<::opengl::ShaderProgram>(
      std::vector<::opengl::ShaderProgram::Shader>{
        {GL_VERTEX_SHADER, get("shader/mesh2d.vs")},
        {GL_FRAGMENT_SHADER, get("shader/text.fs")}
      }
    );
  }
};

} // namespace opengl
