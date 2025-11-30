#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/opengl/services/gui_service.hpp>
#include <axgl/impl/opengl/services/renderer_service.hpp>
#include <axgl/impl/opengl/services/text_service.hpp>

namespace axgl
{
AXGL_DECLARE_REGISTER_SERVICE(impl::opengl::RendererService)
AXGL_DECLARE_REGISTER_SERVICE(impl::opengl::TextService)
AXGL_DECLARE_REGISTER_SERVICE(impl::opengl::GuiService)
} // namespace axgl
