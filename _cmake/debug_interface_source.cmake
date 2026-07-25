#
# Header-only INTERFACE libraries produce no compile commands of their own,
# so clangd (and clang-tidy) cannot resolve includes or give diagnostics on
# their headers when their TUs aren't pulled in transitively from somewhere
# else in the build.
#
# This helper fixes that by giving each header-only target a sibling static
# library (gated to Debug only) that compiles a single .cpp pulling in the
# target's umbrella header. The static lib is EXCLUDE_FROM_ALL and nothing
# links it, so the release build is zero-cost; the Debug build gains one
# compile command per INTERFACE target, which CMake exports into
# _build/Debug/compile_commands.json for clangd to consume.
#
function(add_debug_interface_source target source)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_library(${target}_debug STATIC EXCLUDE_FROM_ALL ${source})
    target_link_libraries(${target}_debug PRIVATE ${target})

    set(options)
    set(one_value_args INCLUDE_DIRECTORY UMBRELLA_HEADER)
    set(multi_value_args LINK_LIBRARIES)
    cmake_parse_arguments(ARG "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    if(ARG_INCLUDE_DIRECTORY)
      file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${ARG_INCLUDE_DIRECTORY}/*.hpp")
      set(header_sources)

      foreach(header IN LISTS headers)
        file(RELATIVE_PATH header_relative "${ARG_INCLUDE_DIRECTORY}" "${header}")
        set(header_source "${CMAKE_CURRENT_BINARY_DIR}/header_sources/${header_relative}.cpp")
        file(GENERATE OUTPUT "${header_source}"
          CONTENT "#include <${ARG_UMBRELLA_HEADER}>\n#include <${header_relative}>\n"
        )
        list(APPEND header_sources "${header_source}")
      endforeach()

      add_library(${target}_headers_debug OBJECT EXCLUDE_FROM_ALL ${header_sources})
      target_link_libraries(${target}_headers_debug PRIVATE ${target} ${ARG_LINK_LIBRARIES})
    endif()
  endif()
endfunction()
