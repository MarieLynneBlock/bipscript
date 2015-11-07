
message(STATUS "CMAKE_CURRENT_SOURCE_DIR : ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "CMAKE_GENERATOR : ${CMAKE_GENERATOR}")

project(bipscript)
cmake_minimum_required(VERSION 2.8)

aux_source_directory("src" SRC_LIST)
aux_source_directory("bindings" SRC_LIST)
aux_source_directory("squirrel/squirrel" SRC_LIST)
aux_source_directory("squirrel/sqstdlib" SRC_LIST)

include_directories("src")
include_directories("bindings")
include_directories("squirrel/include")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

# message(STATUS "SRC_LIST : ${SRC_LIST}")

add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(${PROJECT_NAME} "jack")
target_link_libraries(${PROJECT_NAME} "lilv-0")
