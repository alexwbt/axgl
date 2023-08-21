#include "axgl/opengl/static_resource.h"
#include "axgl/opengl/shader/skybox-shader.h"

enum class ShaderID : uint32_t
{
  kSkybox,
  kPhong,
};

enum class TextureID : uint32_t
{
  kSkybox,
};

void load_shaders()
{
  using namespace axgl::opengl::shader;

  load_shader(static_cast<uint32_t>(ShaderID::kSkybox), std::make_shared<SkyboxShader>());
  load_shader(static_cast<uint32_t>(ShaderID::kPhong), std::make_shared<PhongShader>());
}

void load_textures()
{
  using namespace axgl::opengl;

  std::vector<std::string> skybox_texture_paths{
      "res/textures/skybox/void/right.png",
      "res/textures/skybox/void/left.png",
      "res/textures/skybox/void/top.png",
      "res/textures/skybox/void/bottom.png",
      "res/textures/skybox/void/front.png",
      "res/textures/skybox/void/back.png"
  };
  load_texture(static_cast<uint32_t>(ShaderID::kSkybox),
    Texture::load_cubemap_texture(skybox_texture_paths));
}

std::shared_ptr<axgl::opengl::ShaderProgram> get_shader(ShaderID shaderId)
{
  return axgl::opengl::get_shader(static_cast<uint32_t>(shaderId));
}

std::shared_ptr<axgl::opengl::Texture> get_texture(TextureID textureId)
{
  return axgl::opengl::get_texture(static_cast<uint32_t>(textureId));
}
