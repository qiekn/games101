cmake_minimum_required(VERSION 3.10)
project(Rasterizer)

find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME}
  main.cpp
  rasterizer.cpp
  Triangle.cpp
)

target_link_libraries(${PROJECT_NAME} Eigen3::Eigen)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
