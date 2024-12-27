cmake_minimum_required(VERSION 3.0...3.28)
project(axgl VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/_bin)

# External Dependencies
add_subdirectory(_external/glm)
add_subdirectory(_external/glfw)
add_subdirectory(_external/args)
add_subdirectory(_external/tracy)
add_subdirectory(_external/spdlog)
add_subdirectory(_external/assimp)
add_subdirectory(_external/utfcpp)
add_subdirectory(_external/cpptrace)
add_subdirectory(_external/freetype)
add_subdirectory(_external/flatbuffer)

# Internal Libraries
add_subdirectory(lib/glad)
add_subdirectory(lib/assbin)
add_subdirectory(lib/embedfile)
add_subdirectory(lib/bundlefile)

# AXGL
add_subdirectory(axgl)

# Implementations
add_subdirectory(impl/glfw)
add_subdirectory(impl/opengl)
add_subdirectory(impl/assimp)
add_subdirectory(impl/bundlefile)

# Demonstrations
add_subdirectory(demo/opengl)
