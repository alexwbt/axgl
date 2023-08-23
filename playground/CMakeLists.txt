
add_executable(playground)

file(GLOB_RECURSE source_cpp_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/*.cpp")

target_sources(playground
  PRIVATE
  ${source_cpp_files}
)

target_include_directories(playground
  PRIVATE

  glfw/include
  glad/include
  spdlog/include
  assimp/include
  axgl/include
)

target_link_libraries(playground
  PRIVATE
  glm
  glfw
  glad
  spdlog
  assimp
  axgl
)

include(${CMAKE_SOURCE_DIR}/_cmake/resource.cmake)
add_resource_dir(${CMAKE_CURRENT_SOURCE_DIR}/res)
