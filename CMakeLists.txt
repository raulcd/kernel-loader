cmake_minimum_required(VERSION 3.12)
project(arrow_kernel_loader)


option(ARROW_LINK_SHARED "Link to the Arrow shared library" ON)
# Find Arrow libraries
find_package(Arrow REQUIRED)

# Include directories
include_directories(${ARROW_INCLUDE_DIR})

# Add source files
add_library(arrow_kernel_loader SHARED src/load_kernels.cc
        src/load_kernels.cc
        src/load_kernels.h)

# Link Arrow libraries
if(ARROW_LINK_SHARED)
    target_link_libraries(arrow_kernel_loader PRIVATE Arrow::arrow_shared)
else()
    target_link_libraries(arrow_kernel_loader PRIVATE Arrow::arrow_static)
endif()
