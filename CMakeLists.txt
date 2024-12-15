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

# External Dependencies
add_subdirectory(_external/glm)
add_subdirectory(_external/glfw)
add_subdirectory(_external/args)
add_subdirectory(_external/spdlog)
add_subdirectory(_external/assimp)
add_subdirectory(_external/cpptrace)
add_subdirectory(_external/flatbuffer)

# Internal Libraries
add_subdirectory(lib/glad)
add_subdirectory(lib/embedfile)
add_subdirectory(lib/bundlefile)

# AXGL
add_subdirectory(axgl)

# Implementations
add_subdirectory(impl/glfw)
add_subdirectory(impl/opengl)
add_subdirectory(impl/assimp)

# Demonstrations
add_subdirectory(demo/opengl)
