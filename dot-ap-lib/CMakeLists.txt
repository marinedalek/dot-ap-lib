# CMakeList.txt : CMake project for dot-ap-lib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_library (dot-ap-lib "dot-ap-lib.cpp" "dot-ap-lib.h" "ap.h" "utility.h" "ap.cpp" "acc_dbe.h" "acc_dbe.cpp" "location_palette.h" "location_palette.cpp" "color.h")
target_include_directories(dot-ap-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# TODO: Add tests and install targets if needed.
