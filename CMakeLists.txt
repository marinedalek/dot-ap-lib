﻿# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("dot-ap-lib")

# Include sub-projects.
add_subdirectory ("dot-ap-lib")
target_compile_features(dot-ap-lib PUBLIC cxx_std_23)
add_subdirectory ("demo")
