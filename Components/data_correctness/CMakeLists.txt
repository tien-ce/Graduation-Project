cmake_minimum_required (VERSION 2.8.10)
project(datacorrect_library C)
# Add a shared library target 
add_library(datacorrect SHARED main.c)
# Set version 
set_target_properties(datacorrect PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
)
#Specify the public include directories for dependent targets
target_include_directories(datacorrect PRIVATE Include)
# Optional: Ensure position-independent code for maximum compatibility across platforms
set_target_properties(datacorrect  PROPERTIES POSITION_INDEPENDENT_CODE ON)
install(TARGETS datacorrect DESTINATION ${CMAKE_INSTALL_LIBDIR})
