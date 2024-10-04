cmake_minimum_required(VERSION 3.22)
project(pn_compiler)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall)
add_compile_definitions(NOT_EJUDGE)
add_compile_definitions(PROFILER_ENABLE)

include_directories(include)

set(SOURCES src/main.cpp src/StatementBlock.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})
