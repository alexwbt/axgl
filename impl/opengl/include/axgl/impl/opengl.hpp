#pragma once

#include <axgl/axgl.hpp>

/* Component implementations */

#include <axgl/impl/opengl/component/mesh.hpp>

NAMESPACE_AXGL

#ifdef AXGL_DEFINED_CREATE_COMPONENT_MESH
#error Duplicate definition of component interface::Mesh
#endif
#define AXGL_DEFINED_CREATE_COMPONENT_MESH
template<>
std::shared_ptr<interface::Mesh> interface::RealmService::create_component()
{
  return std::make_shared<impl::OpenglMesh>();
}

NAMESPACE_AXGL_END

#include <axgl/impl/opengl/component/gui_element.hpp>

NAMESPACE_AXGL

#ifdef AXGL_DEFINED_CREATE_COMPONENT_GUI_ELEMENT
#error Duplicate definition of component interface::GuiElement
#endif
#define AXGL_DEFINED_CREATE_COMPONENT_GUI_ELEMENT
// template<>
// std::shared_ptr<interface::GuiElement> interface::RealmService::create_component()
// {
//   return std::make_shared<impl::OpenglGuiElement>();
// }

NAMESPACE_AXGL_END

/* Service implementations */

#include <axgl/impl/opengl/renderer.hpp>
#include <axgl/impl/opengl/gui.hpp>
#include <axgl/impl/opengl/text.hpp>

NAMESPACE_AXGL

template<>
std::shared_ptr<impl::OpenglRendererService> Axgl::use_service()
{
#ifdef AXGL_DEBUG
  if (!has_service_type<impl::GlfwWindowService>("window"))
    throw std::runtime_error("GlfwWindowService is missing, it is required for OpenglRendererService.");
#endif

  // set glfw context
  auto window_service = get_service<impl::GlfwWindowService>("window");
  window_service->set_window_hint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  window_service->set_window_hint(GLFW_CONTEXT_VERSION_MINOR, 3);
  window_service->set_window_hint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // create opengl service
  auto opengl_service = std::make_shared<impl::OpenglRendererService>();
  register_service("renderer", opengl_service);

  return opengl_service;
}

template<>
std::shared_ptr<impl::OpenglGuiService> Axgl::use_service()
{
  // auto text_service = std::make_shared<impl::OpenglTextService>();
  auto gui_service = std::make_shared<impl::OpenglGuiService>();
  // register_service("text", text_service);
  register_service("gui", gui_service);

  return gui_service;
}

NAMESPACE_AXGL_END
