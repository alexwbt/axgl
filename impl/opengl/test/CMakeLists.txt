add_executable(test_opengl_shader_source shader_source_preprocess.cpp)

target_link_libraries(test_opengl_shader_source
  PRIVATE
  axgl_opengl_impl
  doctest::doctest_with_main
)

add_test(NAME test_opengl_shader_source COMMAND test_opengl_shader_source)