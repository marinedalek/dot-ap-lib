cmake_minimum_required(VERSION 3.8)

find_package(raylib CONFIG REQUIRED)

add_executable (demo "demo.cpp")
target_compile_features(demo PRIVATE cxx_std_23)
target_include_directories(demo PRIVATE ${RAYLIB_INCLUDE_DIRS})
target_link_libraries(demo dot-ap-lib ${RAYLIB_LIBRARIES})