#pragma once

#include "axgl/namespace.h"

#include <map>
#include <memory>

#include "axgl/opengl/texture.h"
#include "axgl/opengl/shader_program.h"

NAMESPACE_OPENGL

void load_texture(uint32_t textureId, std::shared_ptr<Texture> texture);
std::shared_ptr<Texture> get_texture(uint32_t textureId);

void load_shader(uint32_t shaderId, std::shared_ptr<ShaderProgram> shaders);
std::shared_ptr<ShaderProgram> get_shader(uint32_t shaderId);

NAMESPACE_OPENGL_END
