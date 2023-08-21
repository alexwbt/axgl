#include "axgl/opengl/static_resource.h"
#include "axgl/opengl/shader/skybox-shader.h"

enum class ShaderID : uint32_t
{
  kSkybox,
};

enum class TextureID : uint32_t
{
  kSkybox,
};

void load_shaders()
{
  using namespace axgl::opengl::shader;

  load_shader(static_cast<uint32_t>(ShaderID::kSkybox),
    std::make_shared<SkyboxShader>());
}

void load_textures()
{
  using namespace axgl::opengl;

  std::vector<std::string> skybox_texture_paths{
      "res/textures/skybox/right.jpg",
      "res/textures/skybox/left.jpg",
      "res/textures/skybox/top.jpg",
      "res/textures/skybox/bottom.jpg",
      "res/textures/skybox/front.jpg",
      "res/textures/skybox/back.jpg"
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
