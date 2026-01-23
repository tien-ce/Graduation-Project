cmake_minimum_required (VERSION 2.8.10)
project(air_485_library C)
#Add a shared library target 
add_library(air_485 SHARED air_rs485.c)
#Specify the public include directories for dependent targets
target_include_directories(air_485 PRIVATE Include)
# Optional: Ensure position-independent code for maximum compatibility across platforms
set_target_properties(air_485  PROPERTIES POSITION_INDEPENDENT_CODE ON)
install(TARGETS air_485 DESTINATION ${CMAKE_INSTALL_BINDIR})
