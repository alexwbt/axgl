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
function(add_header_proxy target source)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_library(${target}_proxy STATIC EXCLUDE_FROM_ALL ${source})
    target_link_libraries(${target}_proxy PRIVATE ${target})

    #
    # If INCLUDE_DIRECTORY is supplied, generate one synthetic .cpp per header
    # under it. Each generated TU includes the optional UMBRELLA_HEADER first
    # (so forward declarations resolve) and then the header itself, giving
    # clangd a per-header compile command in compile_commands.json that is
    # not subject to its filename-closest heuristic.
    #
    # Without this, an axgl header shadowed by a same-named source elsewhere
    # in the build (e.g. axgl/.../gui/context.hpp vs. _external/glfw/context.c)
    # would have clangd borrow the third-party compile command, which lacks
    # the AXGL include paths and -D macros needed to resolve <axgl/...>.
    #
    set(one_value_args INCLUDE_DIRECTORY UMBRELLA_HEADER)
    cmake_parse_arguments(ARG "" "${one_value_args}" "" ${ARGN})

    if(ARG_INCLUDE_DIRECTORY)
      file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${ARG_INCLUDE_DIRECTORY}/*.hpp")
      set(header_sources)

      if(ARG_UMBRELLA_HEADER)
        set(umbrella_header_line "#include <${ARG_UMBRELLA_HEADER}>\n")
      else()
        set(umbrella_header_line "")
      endif()

      foreach(header IN LISTS headers)
        file(RELATIVE_PATH header_relative "${ARG_INCLUDE_DIRECTORY}" "${header}")
        set(header_source "${CMAKE_CURRENT_BINARY_DIR}/header_sources/${header_relative}.cpp")
        file(GENERATE OUTPUT "${header_source}"
          CONTENT "${umbrella_header_line}\n#include <${header_relative}>\n"
        )
        list(APPEND header_sources "${header_source}")
      endforeach()

      add_library(${target}_header_proxy OBJECT EXCLUDE_FROM_ALL ${header_sources})
      target_link_libraries(${target}_header_proxy PRIVATE ${target})
    endif()
  endif()
endfunction()
