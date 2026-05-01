cmake_minimum_required(VERSION 3.20)

project(ai_hardware_lab LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(AIHW_ENABLE_NATIVE "Enable local CPU tuning flags such as -march=native" OFF)

add_library(aihw_cpu
  src/matmul.cpp
)

target_include_directories(aihw_cpu
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
  target_compile_options(aihw_cpu PRIVATE /W4)
else()
  target_compile_options(aihw_cpu PRIVATE -Wall -Wextra -Wpedantic)
  if(AIHW_ENABLE_NATIVE)
    target_compile_options(aihw_cpu PRIVATE -march=native)
  endif()
endif()

add_executable(test_matmul
  tests/test_matmul.cpp
)
target_link_libraries(test_matmul PRIVATE aihw_cpu)

add_executable(bench_matmul
  benchmarks/bench_matmul.cpp
)
target_link_libraries(bench_matmul PRIVATE aihw_cpu)

add_executable(bench_memory_access
  benchmarks/bench_memory_access.cpp
)

enable_testing()
add_test(NAME matmul_correctness COMMAND test_matmul)
