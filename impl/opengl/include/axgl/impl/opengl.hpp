#pragma once

#include <axgl/axgl.hpp>

/* Component implementations */

#include <axgl/impl/opengl/component/mesh.hpp>

NAMESPACE_AXGL

#ifdef AXGL_DEFINED_CREATE_COMPONENT_MESH
#error Duplicate definition of component interface::Mesh
#endif
#define AXGL_DEFINED_CREATE_COMPONENT_MESH
template <> inline std::shared_ptr<interface::component::Mesh> interface::RealmService::create_component()
{
  return std::make_shared<impl::component::OpenglMesh>();
}

NAMESPACE_AXGL_END

#include <axgl/impl/opengl/entity/gui_element.hpp>

NAMESPACE_AXGL

#ifdef AXGL_DEFINED_CREATE_ENTITY_GUI_ELEMENT
#error Duplicate definition of entity interface::entity::GuiElement
#endif
#define AXGL_DEFINED_CREATE_ENTITY_GUI_ELEMENT
template <> inline std::shared_ptr<interface::entity::GuiElement> interface::RealmService::create_entity()
{
  return std::make_shared<impl::entity::OpenglGuiElement>();
}

NAMESPACE_AXGL_END

/* Service implementations */

#include <axgl/impl/opengl/renderer.hpp>
#include <axgl/impl/opengl/gui.hpp>
#include <axgl/impl/opengl/text.hpp>

NAMESPACE_AXGL

template <> inline std::shared_ptr<impl::OpenglRendererService> Axgl::use_service()
{
#ifdef AXGL_DEBUG
  if (!has_service_type<impl::GlfwWindowService>("window"))
    throw std::runtime_error("GlfwWindowService is missing, it is required for OpenglRendererService.");
#endif

  // set glfw context
  const auto window_service = get_service<impl::GlfwWindowService>("window");
  window_service->set_window_hint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  window_service->set_window_hint(GLFW_CONTEXT_VERSION_MINOR, 3);
  window_service->set_window_hint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // register opengl service
  const auto opengl_service = std::make_shared<impl::OpenglRendererService>();
  register_service("renderer", std::static_pointer_cast<interface::RendererService>(opengl_service));

  // register opengl related services
  const auto gui_service = std::make_shared<impl::OpenglGuiService>();
  register_service("gui", std::static_pointer_cast<interface::GuiService>(gui_service));
  const auto text_service = std::make_shared<impl::OpenglTextService>();
  register_service("text", text_service);

  return opengl_service;
}

NAMESPACE_AXGL_END
