# CMakeList.txt : CMake project for dot-ap-lib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_library (dot-ap-lib "dot-ap-lib.cpp" "dot-ap-lib.h" "ap.h" "utility.h" "ap.cpp")
target_include_directories(dot-ap-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# TODO: Add tests and install targets if needed.
