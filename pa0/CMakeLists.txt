cmake_minimum_required (VERSION 2.8.11)
project (Transformation)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Eigen3 REQUIRED)

add_executable (${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} Eigen3::Eigen)
