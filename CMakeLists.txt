cmake_minimum_required(VERSION 3.12)
project(arrow_kernel_loader)


option(ARROW_LINK_SHARED "Link to the Arrow shared library" ON)
# Find Arrow libraries
find_package(Arrow REQUIRED)

# Include directories
include_directories(${ARROW_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/vendored)
include_directories(${CMAKE_BINARY_DIR}/arrow_vendored/cpp/src/)

# Add source files
add_library(arrow_kernel_loader SHARED
        ${CMAKE_BINARY_DIR}/arrow_vendored/cpp/src/arrow/compute/kernels/chunked_internal.cc
        ${CMAKE_BINARY_DIR}/arrow_vendored/cpp/src/arrow/chunk_resolver.cc
        ${CMAKE_BINARY_DIR}/arrow_vendored/cpp/src/arrow/compute/api_vector.cc
        ${CMAKE_BINARY_DIR}/arrow_vendored/cpp/src/arrow/compute/kernels/vector_rank.cc
        src/load_kernels.cc)

# Link Arrow libraries
if(ARROW_LINK_SHARED)
    target_link_libraries(arrow_kernel_loader PRIVATE Arrow::arrow_shared)
else()
    target_link_libraries(arrow_kernel_loader PRIVATE Arrow::arrow_static)
endif()
install(TARGETS arrow_kernel_loader
        DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/load_kernels.h"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/include/arrow/compute/internal/vendored")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/arrow_vendored/cpp/src/arrow/compute/api_vector.h"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/include/arrow/compute/internal/vendored")
