#pragma once

#include <memory>

#include <axgl/common.hpp>
#include <axgl/interface/gui.hpp>
#include <axgl/interface/realm.hpp>

#ifndef AXGL_DEFINED_CREATE_COMPONENT_GUI_ELEMENT
#error Implementation of interface::GuiElement must be defined before using __FILE__
#endif

NAMESPACE_AXGL_IMPL

class OpenglGuiService : public interface::GuiService
{
private:
  std::shared_ptr<interface::RealmService> realm_service_;

  std::vector<std::shared_ptr<interface::GuiElement>> pages_;

public:
  void initialize() override
  {
    auto context = get_context();
    realm_service_ = context->axgl->realm_service();
  }

  std::shared_ptr<interface::GuiElement> create_page() override
  {
    auto page = realm_service_->create_component<interface::GuiElement>();
    pages_.push_back(page);
    return page;
  }

  std::shared_ptr<interface::GuiElement> create_element() const override
  {
    return realm_service_->create_component<interface::GuiElement>();
  }
};

NAMESPACE_AXGL_IMPL_END
