function(add_tests target_prefix test_dir)
  if(AXGL_BUILD_TESTS)
    cmake_parse_arguments(ARG "" "" "DEPS" ${ARGN})

    file(GLOB TEST_SOURCES "${test_dir}/*.cpp")

    foreach(test_source IN LISTS TEST_SOURCES)
      get_filename_component(test_name "${test_source}" NAME_WLE)
      set(test_target ${target_prefix}_${test_name})

      add_executable(${test_target} ${test_source})
      target_link_libraries(${test_target}
        PRIVATE
        doctest::doctest_with_main
        ${ARG_DEPS}
      )

      add_test(NAME ${test_target} COMMAND ${test_target})
      add_dependencies(axgl_tests ${test_target})
    endforeach()
  endif()
endfunction()
