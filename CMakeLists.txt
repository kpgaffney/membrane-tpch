cmake_minimum_required(VERSION 3.16)
cmake_policy(VERSION 3.24)

project(membrane_tpch)

set(CMAKE_CXX_STANDARD 20)

if (NOT (DEFINED DUCKDB_INCLUDE_DIR AND DEFINED DUCKDB_LIBRARY))
    message(FATAL_ERROR "must provide DUCKDB_INCLUDE_DIR and DUCKDB_LIBRARY")
endif ()

include(FetchContent)

FetchContent_Declare(
        ittapi
        GIT_REPOSITORY https://github.com/intel/ittapi.git
        GIT_TAG v3.24.3
)
FetchContent_MakeAvailable(ittapi)

FetchContent_Declare(
        tbb
        GIT_REPOSITORY https://github.com/oneapi-src/oneTBB.git
        GIT_TAG v2021.10.0
)
FetchContent_MakeAvailable(tbb)

add_executable(run src/run.cpp)
target_include_directories(run PRIVATE ${DUCKDB_INCLUDE_DIR})
target_link_libraries(run PRIVATE ${DUCKDB_LIBRARY} ittnotify TBB::tbb)

file(COPY src/sql DESTINATION ${CMAKE_BINARY_DIR})
