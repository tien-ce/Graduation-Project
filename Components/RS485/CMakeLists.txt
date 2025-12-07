cmake_minimum_required (VERSION 3.28.3)
project(air_485_library C)
add_library(air_485 STATIC air_rs485.c)
target_include_directories(air_485 PRIVATE Include)