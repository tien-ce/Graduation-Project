cmake_minimum_required (VERSION 2.8.10)
project(pmsensor_library C)
# Add a shared library target 
add_library(pmsensor SHARED air_rs485.c)
# Set version 
set_target_properties(pmsensor PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
)
#Specify the public include directories for dependent targets
target_include_directories(pmsensor PRIVATE Include)
# Optional: Ensure position-independent code for maximum compatibility across platforms
set_target_properties(pmsensor  PROPERTIES POSITION_INDEPENDENT_CODE ON)
install(TARGETS pmsensor DESTINATION ${CMAKE_INSTALL_LIBDIR})
