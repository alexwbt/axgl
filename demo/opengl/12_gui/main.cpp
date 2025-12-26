#include "setup.hpp"

int main()
{
  axgl::Axgl axgl;
  setup(axgl);

  const auto& gui_service = axgl.gui_service();

  const auto page = gui_service->create_page();
  gui_service->set_main_ui(page);

  {
    const auto element1 = gui_service->create_element();
    page->elements().add(element1);
    auto& prop1 = element1->property();
    prop1.position = glm::vec2(10.0f, 10.0f);
    prop1.size = glm::vec2(300.0f, 300.0f);
    prop1.color = glm::vec4(0.39f, 0.58f, 0.93f, 1.0f);
    {
      const auto element2 = gui_service->create_element();
      element1->children().add(element2);
      auto& prop2 = element2->property();
      prop2.position = glm::vec2(10.0f, 10.0f);
      prop2.size = glm::vec2(300.0f, 300.0f);
      prop2.color = glm::vec4(1.0f, 0.5f, 0.3f, 1.0f);
      {
        const auto element3 = gui_service->create_element();
        element2->children().add(element3);
        auto& prop3 = element3->property();
        prop3.position = glm::vec2(-20.0f, 10.0f);
        prop3.size = glm::vec2(300.0f, 10.0f);
        prop3.color = glm::vec4(0.0f, 0.5f, 0.3f, 1.0f);
      }
    }
  }

  axgl.run();
  axgl.terminate();
}
