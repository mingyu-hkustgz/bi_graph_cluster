cmake_minimum_required(VERSION 3.2)
project(bi_graph_cluster)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-static")

include_directories(include)

find_package(OpenMP)

if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
else()
    message(FATAL_ERROR "no OpenMP supprot")
endif()

# windows dependency
# include_directories(D:/software/boost_1_78_0/)
# include_directories(D:/software/eigen-3.4.0)
# link_directories(D:/software/boost_1_78_0/libs)

add_definitions(-std=c++17 -Ofast)

add_subdirectory(src)
add_subdirectory(test)


