#pragma once

#include "axgl/namespace.h"

#include <map>
#include <memory>

#include "axgl/opengl/texture.h"
#include "axgl/opengl/shader_program.h"

NAMESPACE_OPENGL

void load_shader(uint32_t shaderId, std::shared_ptr<ShaderProgram> shaders);
std::shared_ptr<ShaderProgram> get_shader(uint32_t shaderId);

std::shared_ptr<Texture> load_texture(const std::string& texture, Texture::Type type = Texture::kDiffuse);
std::shared_ptr<Texture> load_skybox(const std::string& texture, Texture::Type type = Texture::kDiffuse);

NAMESPACE_OPENGL_END
