
add_executable(playground)

target_sources(playground
  PRIVATE
  ./src/main.cpp
)

target_include_directories(playground
  PRIVATE

  glfw/include
  glad/include
  spdlog/include
  axgl/include
)

target_link_libraries(playground
  PRIVATE
  glm
  glfw
  glad
  spdlog
  axgl
)
