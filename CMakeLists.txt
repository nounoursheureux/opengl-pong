cmake_minimum_required(VERSION 2.6)
project(opengl-pong)

set(SOURCES src/glad.c src/main.c src/render.c src/utils.c)
include_directories("${PROJECT_SOURCE_DIR}/include")
add_executable(opengl-pong ${SOURCES})

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("${PROJECT_SOURCE_DIR}/external/glfw")

target_link_libraries(opengl-pong glfw)

add_custom_command(TARGET opengl-pong PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res ${CMAKE_BINARY_DIR}/res)
