cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(dbtrain-lab)

include_directories(${PROJECT_SOURCE_DIR}/src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

file(GLOB_RECURSE THDB_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cc)
add_library(thdb_shared SHARED ${THDB_SOURCES})

target_link_libraries(thdb_shared antlr4-runtime)

file(GLOB LABS ${PROJECT_SOURCE_DIR}/test/lab*)
foreach(lab_dir ${LABS})
    file(GLOB_RECURSE LAB_SOURCES ${lab_dir}/*.cc)
    get_filename_component(lab ${lab_dir} NAME)
    add_executable(${lab}_test ${LAB_SOURCES})
    target_link_libraries(${lab}_test thdb_shared gtest pthread)
endforeach(lab_dir ${LABS})
