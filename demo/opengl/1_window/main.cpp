#include <iostream>

#ifdef AXGL_DEBUG
int main()
{
  std::cout << "4 Debug" << std::endl;
}
#else
int main()
{
  std::cout << "4 Release" << std::endl;
}
#endif

// #include <axgl/axgl.hpp>
// #include <axgl/impl/glfw.hpp>

// int main()
// {
//   axgl::Axgl axgl;

//   // register window service
//   auto window_service = axgl.use_service<axgl::impl::GlfwWindowService>();
//   // create window
//   auto window = window_service->create_window();
//   window->set_title("Hello Window!");

//   axgl.run();
// }
