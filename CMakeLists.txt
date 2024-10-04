cmake_minimum_required(VERSION 3.22)
project(pn_compiler)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall)

set(EJUDGE_MODE OFF)
if(NOT EJUDGE_MODE)
  add_compile_definitions(NOT_EJUDGE)
  add_compile_definitions(PROFILER_ENABLE)
endif(NOT EJUDGE_MODE)

include_directories(include)

set(SOURCES src/main.cpp src/StatementBlock.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

include(FetchContent)
FetchContent_Declare(
  profiler
  GIT_REPOSITORY https://github.com/Lolitron-0/Profiler
  GIT_TAG master)
FetchContent_MakeAvailable(Profiler)

target_link_libraries(${PROJECT_NAME} PRIVATE Profiler)
