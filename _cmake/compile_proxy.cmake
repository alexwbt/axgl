#
# Adds a static library that compiles every .hpp under INCLUDE_DIRECTORY (plus optional HEADER_FILES)
# so clangd/clang-tidy get a compile command per header.
#
function(add_compile_proxy target include_directory)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    cmake_parse_arguments(ARG "" "" "HEADER_FILES" ${ARGN})

    # add header sources
    file(GLOB_RECURSE headers "${include_directory}/*.hpp")
    list(APPEND headers ${ARG_HEADER_FILES})

    set(source_files)

    foreach(header IN LISTS headers)
      file(RELATIVE_PATH header_relative_path "${include_directory}" "${header}")
      get_filename_component(header_dir "${header_relative_path}" DIRECTORY)
      get_filename_component(header_name "${header_relative_path}" NAME_WE)
      set(header_source "${CMAKE_CURRENT_BINARY_DIR}/compile_proxy/${header_dir}/${header_name}.cpp")
      file(GENERATE OUTPUT "${header_source}"
        CONTENT "#include <${header_relative_path}>\n"
      )
      list(APPEND source_files "${header_source}")
    endforeach()

    add_library(${target}_proxy STATIC EXCLUDE_FROM_ALL ${source_files})
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
      get_target_property(target_include_dirs ${target} INTERFACE_INCLUDE_DIRECTORIES)

      if(target_source_libs)
        target_link_libraries(${target}_proxy PRIVATE ${target_source_libs})
      endif()

      if(target_interface_libs)
        target_link_libraries(${target}_proxy PRIVATE ${target_interface_libs})
      endif()

      if(target_include_dirs)
        target_include_directories(${target}_proxy PRIVATE ${target_include_dirs})
      endif()
    endif()
  endif()
endfunction()
