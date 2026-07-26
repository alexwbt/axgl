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
function(add_interface_proxy target source)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_library(${target}_proxy STATIC EXCLUDE_FROM_ALL ${source})
    target_link_libraries(${target}_proxy PRIVATE ${target})
  endif()
endfunction()

#
# Adds the headers of a header-only target as synthesized compile units of an
# existing proxy target, so clangd/clang-tidy get diagnostics for every header
# individually rather than only for the umbrella header.
#
# For each .hpp under INCLUDE_DIRECTORY it generates a tiny .cpp (in the build
# dir) that includes the umbrella header (if any) plus the header itself, then
# adds those generated .cpps as sources of `proxy_target`. Debug-gated like
# add_interface_proxy: in release this is a no-op, so the release build stays
# zero-cost.
#
function(add_header_sources proxy_target)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Parse optional one-value keyword arguments into ARG_INCLUDE_DIRECTORY /
    # ARG_UMBRELLA_HEADER.
    set(one_value_args INCLUDE_DIRECTORY UMBRELLA_HEADER)
    cmake_parse_arguments(ARG "" "${one_value_args}" "" ${ARGN})

    if(ARG_INCLUDE_DIRECTORY)
      # Recursively collect every .hpp under the include dir. CONFIGURE_DEPENDS
      # makes CMake re-run when headers are added/removed so the generated
      # source set stays in sync.
      file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${ARG_INCLUDE_DIRECTORY}/*.hpp")
      set(header_sources)

      # Build the umbrella-header include line once; empty when none was given
      # so generated .cpps include the target header standalone.
      if(ARG_UMBRELLA_HEADER)
        set(umbrella_header_line "#include <${ARG_UMBRELLA_HEADER}>\n")
      else()
        set(umbrella_header_line "")
      endif()

      # Emit one synthesized .cpp per header. The path mirrors the header's
      # relative location under INCLUDE_DIRECTORY so generated files don't
      # collide. file(GENERATE) runs at build time, so it picks up headers
      # added after configure without needing a reconfigure.
      foreach(header IN LISTS headers)
        file(RELATIVE_PATH header_relative "${ARG_INCLUDE_DIRECTORY}" "${header}")
        set(header_source "${CMAKE_CURRENT_BINARY_DIR}/header_sources/${header_relative}.cpp")
        file(GENERATE OUTPUT "${header_source}"
          CONTENT "${umbrella_header_line}#include <${header_relative}>\n"
        )
        list(APPEND header_sources "${header_source}")
      endforeach()

      # Attach the generated sources and the include dir so the proxy compiles
      # each header with the right search paths, exporting one compile command
      # per header into compile_commands.json for clangd/clang-tidy.
      target_sources(${proxy_target} PRIVATE ${header_sources})
      target_include_directories(${proxy_target} PRIVATE ${ARG_INCLUDE_DIRECTORY})
    endif()
  endif()
endfunction()
