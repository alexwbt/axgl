add_executable(demo_window)
target_sources(demo_window PRIVATE "main.cpp")
target_link_libraries(demo_window PRIVATE axgl_glfw_impl)
