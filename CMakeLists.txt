cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(dbtrain-lab)

include_directories(${PROJECT_SOURCE_DIR}/src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

file(GLOB_RECURSE THDB_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cc)
add_library(thdb_shared SHARED ${THDB_SOURCES})

file(GLOB_RECURSE TEST_SOURCES ${PROJECT_SOURCE_DIR}/test/*.cc)
add_executable(tests ${TEST_SOURCES})

target_link_libraries(thdb_shared antlr4-runtime)
target_link_libraries(tests thdb_shared gtest pthread)
