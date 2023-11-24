cmake_minimum_required(VERSION 3.26)
project(mpiTest)

set(CMAKE_C_COMPILER  "mpicc")

# Add MPI Package to Project
find_package(MPI REQUIRED)

add_executable(mpiTest main.c)

# Link MPI libraries
target_link_libraries(mpiTest PRIVATE MPI::MPI_CXX)