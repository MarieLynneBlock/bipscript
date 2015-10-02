
message(STATUS "CMAKE_GENERATOR : ${CMAKE_GENERATOR}")
message(STATUS "CMAKE_CURRENT_SOURCE_DIR : ${CMAKE_CURRENT_SOURCE_DIR}")

project(bipscript)
cmake_minimum_required(VERSION 2.8)

aux_source_directory(. SRC_LIST)
aux_source_directory("bindings" SRC_LIST)
#aux_source_directory("squirrel" SRC_LIST)

include_directories(".")
include_directories("squirrel")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

# message(STATUS "SRC_LIST : ${SRC_LIST}")

add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(${PROJECT_NAME} "jack")
target_link_libraries(${PROJECT_NAME} "lilv-0")
target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/squirrel/libsquirrel.a)
target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/squirrel/libsqstdlib.a)
