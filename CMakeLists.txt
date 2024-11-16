cmake_minimum_required(VERSION 3.0...3.28)
project(axgl VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

if(UNIX)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/_bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# add_subdirectory(_external/glm)
# add_subdirectory(_external/assimp)
# add_subdirectory(_external/flatbuffer)
add_subdirectory(_external/glfw)
add_subdirectory(_external/spdlog)
add_subdirectory(_external/args)

add_subdirectory(axgl)
add_subdirectory(opengl_demo)
