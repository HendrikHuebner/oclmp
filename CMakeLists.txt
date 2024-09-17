cmake_minimum_required(VERSION 3.10)
project(OCLMP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenCL REQUIRED)
set(${GMP_INCLUDE_DIR} "/usr/include/x86_64-linux-gnu")

add_library(OCLMP 
    src/oclmp.cpp
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

add_executable(oclmpests
    tests/test_ops.cpp
    tests/test_add.cpp
)

target_link_libraries(oclmpests PRIVATE gtest gtest_main OCLMP)
target_include_directories(oclmpests PUBLIC src)

add_test(NAME OCLMPTests COMMAND oclmpests)
