cmake_minimum_required(VERSION 3.10)
project(RayTracing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS "include/*.h")

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE include)
