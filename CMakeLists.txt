cmake_minimum_required(VERSION 3.10)
project(OCLMP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenCL REQUIRED)
find_package(OpenMP REQUIRED)

set(${GMP_INCLUDE_DIR} "/usr/include/x86_64-linux-gnu")

add_library(OCLMP 
    src/oclmp.cpp
    src/computation.cpp
    src/kernels.cpp
    src/types.cpp
    src/util.cpp
)

target_include_directories(OCLMP PRIVATE ${OpenCL_INCLUDE_DIRS} ${GMP_INCLUDE_DIR})

target_link_libraries(OCLMP PRIVATE ${OpenCL_LIBRARIES} gmp)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.12.1.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(oclmptests
    #tests/test_ops.cpp
    tests/test_add.cpp
    tests/test_mul.cpp
    #tests/perf/perf_test.cpp
)

target_link_libraries(oclmptests PUBLIC OpenMP::OpenMP_CXX)

target_link_libraries(oclmptests PRIVATE gtest gtest_main OCLMP)
target_include_directories(oclmptests PUBLIC src)

add_test(NAME OCLMPTests COMMAND oclmptests)
