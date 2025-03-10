cmake_minimum_required(VERSION 3.12)
project(kernel_loader_tests)


option(ARROW_LINK_SHARED "Link to the Arrow shared library" ON)
# Find Arrow libraries
find_package(Arrow REQUIRED)
find_package(ArrowAcero REQUIRED)
# Find arrow_kernel_loader library
add_library(arrow_kernel_loader SHARED IMPORTED)
set_target_properties(arrow_kernel_loader PROPERTIES
        IMPORTED_LOCATION ${CMAKE_INSTALL_PREFIX}/lib/libarrow_kernel_loader.so)

# Include directories
include_directories(${ARROW_INCLUDE_DIR})
include_directories(${CMAKE_BINARY_DIR}/src)
include_directories(${CMAKE_INSTALL_PREFIX}/include)

message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")

# Add source files for the test executable
set(TEST_SOURCES
    test_main.cc
)
# Add the test executable
add_executable(kernel_loader_tests ${TEST_SOURCES})

# Link Arrow libraries and arrow_kernel_loader to the test executable
if(ARROW_LINK_SHARED)
    target_link_libraries(kernel_loader_tests PRIVATE Arrow::arrow_shared ArrowAcero::arrow_acero_shared arrow_kernel_loader)
else()
    target_link_libraries(kernel_loader_tests PRIVATE Arrow::arrow_static ArrowAcero::arrow_acero_static arrow_kernel_loader)
endif()
