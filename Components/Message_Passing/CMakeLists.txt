cmake_minimum_required (VERSION 3.28.3)
project(msg_pa_library C)
add_library(msg_pa STATIC msg.c)
target_include_directories(msg_pa PRIVATE Include)