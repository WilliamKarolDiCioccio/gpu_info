# The Flutter tooling requires that developers have CMake 3.10 or later
# installed. You should not increase this version, as doing so will cause
# the plugin to fail to compile for some customers of the plugin.
cmake_minimum_required(VERSION 3.10)

project(gpu_info_library VERSION 0.0.1 LANGUAGES C)

add_library(gpu_info SHARED
  "gpu_info.c"
)

set_target_properties(gpu_info PROPERTIES
  PUBLIC_HEADER gpu_info.h
  OUTPUT_NAME "gpu_info"
)

target_compile_definitions(gpu_info PUBLIC DART_SHARED_LIB)
