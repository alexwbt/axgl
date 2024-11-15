# Add OpenSSL as an external project
include(ExternalProject)
ExternalProject_Add(openssl
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/openssl
  BUILD_COMMAND ${CMAKE_COMMAND} --build .
  INSTALL_COMMAND ""
)

# Include OpenSSL header directories
ExternalProject_Get_Property(openssl SOURCE_DIR)
include_directories(${SOURCE_DIR}/include)

# Link against OpenSSL libraries
ExternalProject_Get_Property(openssl BINARY_DIR)
link_directories(${BINARY_DIR})

# Add your project's targets and rules
add_executable(your_project_name main.cpp)

# Link your project against OpenSSL
target_link_libraries(your_project_name
  ${BINARY_DIR}/libssl.a
  ${BINARY_DIR}/libcrypto.a
)
