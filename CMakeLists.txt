cmake_minimum_required(VERSION 3.10)
project(OCLMP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCL REQUIRED)
set(${GMP_INCLUDE_DIR} "/usr/include/x86_64-linux-gnu")

add_executable(OCLMP 
    src/main.cpp
    src/types.cpp
)

target_include_directories(OCLMP PRIVATE ${OpenCL_INCLUDE_DIRS} ${GMP_INCLUDE_DIR})

target_link_libraries(OCLMP PRIVATE ${OpenCL_LIBRARIES} gmp)
