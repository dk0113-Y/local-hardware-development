cmake_minimum_required(VERSION 3.20)

project(ai_hardware_lab LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(AIHW_ENABLE_NATIVE "Enable local CPU tuning flags such as -march=native" OFF)

if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
  if(AIHW_ENABLE_NATIVE)
    add_compile_options(-march=native)
  endif()
endif()

add_library(aihw_runtime
  src/tensor.cpp
  src/ops.cpp
  src/cpu_affinity.cpp
)

target_include_directories(aihw_runtime
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(test_tiny_runtime
  tests/test_tiny_runtime.cpp
)
target_link_libraries(test_tiny_runtime PRIVATE aihw_runtime)

add_executable(tiny_mlp
  examples/tiny_mlp.cpp
)
target_link_libraries(tiny_mlp PRIVATE aihw_runtime)

add_executable(bench_tiny_runtime
  benchmarks/bench_tiny_runtime.cpp
)
target_link_libraries(bench_tiny_runtime PRIVATE aihw_runtime)

enable_testing()
add_test(NAME tiny_runtime_correctness COMMAND test_tiny_runtime)
