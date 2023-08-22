// #include <memory>

#include "axgl/opengl/static_resource.h"
#include "axgl/opengl/shader/skybox-shader.h"

enum class ShaderID : uint32_t
{
  kSkybox,
  kPhong,
};

void load_shaders()
{
  using namespace axgl::opengl::shader;

  load_shader(static_cast<uint32_t>(ShaderID::kSkybox), std::make_shared<SkyboxShader>());
  load_shader(static_cast<uint32_t>(ShaderID::kPhong), std::make_shared<PhongShader>());
}

void load_textures()
{
  axgl::opengl::load_skybox("skybox/void");
}

template<class ShaderType>
std::shared_ptr<ShaderType> get_shader(ShaderID shaderId)
{
  return std::static_pointer_cast<ShaderType>(axgl::opengl::get_shader(static_cast<uint32_t>(shaderId)));
}
