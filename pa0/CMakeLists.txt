cmake_minimum_required (VERSION 2.8.11)
project (Transformation)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Eigen3 REQUIRED)

add_executable (Transformation main.cpp)

target_link_libraries(Transformation Eigen3::Eigen)
message(STATUS "Eigen3 include dir: ${EIGEN3_INCLUDE_DIR}")
