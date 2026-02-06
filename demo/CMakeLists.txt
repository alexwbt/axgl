
option(AXGL_DEMO_CONSERVATIVE "Use conservative mode for axgl demo" ON)
option(AXGL_DEMO_USE_OPENGL_IMPL "Use OpenGL implementation for axgl demo" ON)

message("AXGL_DEMO_CONSERVATIVE: ${AXGL_DEMO_CONSERVATIVE}")
message("AXGL_DEMO_USE_OPENGL_IMPL: ${AXGL_DEMO_USE_OPENGL_IMPL}")

if(AXGL_DEMO_USE_OPENGL_IMPL)
  add_compile_definitions(AXGL_DEMO_USE_OPENGL_IMPL)
endif()
if(AXGL_DEMO_CONSERVATIVE)
  add_compile_definitions(AXGL_DEMO_CONSERVATIVE)
endif()


file(GLOB V_GLOB LIST_DIRECTORIES true "*")

foreach(item ${V_GLOB})
  if(IS_DIRECTORY ${item}
    AND NOT ${item} STREQUAL "${CMAKE_BINARY_DIR}/CMakeFiles"
    AND EXISTS "${item}/CMakeLists.txt")
    add_subdirectory(${item})
  endif()
endforeach()
