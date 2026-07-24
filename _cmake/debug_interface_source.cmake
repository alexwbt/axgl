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
  endif()
endfunction()
