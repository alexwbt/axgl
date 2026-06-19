
file(GLOB V_GLOB LIST_DIRECTORIES true "*")

foreach (item ${V_GLOB})
  if (IS_DIRECTORY ${item}
    AND NOT ${item} STREQUAL "${CMAKE_BINARY_DIR}/CMakeFiles"
    AND EXISTS "${item}/CMakeLists.txt")
    add_subdirectory(${item})
  endif ()
endforeach ()
