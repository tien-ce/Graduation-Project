cmake_minimum_required (VERSION 2.8.10)
project(alert_library C)
# Add a shared library target 
add_library(alert SHARED main.c)
# Set version 
set_target_properties(alert PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
)
#Specify the public include directories for dependent targets
target_include_directories(alert PRIVATE Include)
# Force the linker to include ALL code from the static archive
target_link_libraries(alert
    PRIVATE
    gpiod 
)
# Optional: Ensure position-independent code for maximum compatibility across platforms
set_target_properties(alert  PROPERTIES POSITION_INDEPENDENT_CODE ON)
install(TARGETS alert DESTINATION ${CMAKE_INSTALL_LIBDIR})
