cmake_minimum_required(VERSION 3.10)
project(RayTracing)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/scene.cpp
    src/renderer.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)
