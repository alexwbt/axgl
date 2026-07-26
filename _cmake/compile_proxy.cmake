#
# Adds a static library that compiles every .hpp
# under INCLUDE_DIRECTORY (plus optional HEADER_FILES) so clangd/clang-tidy
# get a compile command per header.
#
# target            - target the proxy mirrors.
# include_directory - *.hpp glob root and relative-path base for generated cpp files.
# HEADER_FILES      - extra headers outside INCLUDE_DIRECTORY.
#
function(add_compile_proxy target include_directory)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    cmake_parse_arguments(ARG "" "" "HEADER_FILES" ${ARGN})

    # add header sources
    file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${include_directory}/*.hpp")
    list(APPEND headers ${ARG_HEADER_FILES})

    set(header_sources)

    foreach(header IN LISTS headers)
      file(RELATIVE_PATH header_relative "${include_directory}" "${header}")
      set(header_source "${CMAKE_CURRENT_BINARY_DIR}/header_sources/${header_relative}.cpp")
      file(GENERATE OUTPUT "${header_source}"
        CONTENT "#include <${header_relative}>\n"
      )
      list(APPEND header_sources "${header_source}")
    endforeach()

    add_library(${target}_proxy STATIC EXCLUDE_FROM_ALL ${header_sources})
    target_include_directories(${target}_proxy PRIVATE ${include_directory})

    # link dependencies
    get_target_property(target_type ${target} TYPE)

    if(target_type STREQUAL "INTERFACE_LIBRARY")
      # link itself if it's an interface lib
      target_link_libraries(${target}_proxy PRIVATE ${target})
    else()
      # copy dependencies otherwise
      get_target_property(target_source_libs ${target} LINK_LIBRARIES)
      get_target_property(target_interface_libs ${target} INTERFACE_LINK_LIBRARIES)

      if(target_source_libs)
        target_link_libraries(${target}_proxy PRIVATE ${target_source_libs})
      endif()

      if(target_interface_libs)
        target_link_libraries(${target}_proxy PRIVATE ${target_interface_libs})
      endif()
    endif()
  endif()
endfunction()
