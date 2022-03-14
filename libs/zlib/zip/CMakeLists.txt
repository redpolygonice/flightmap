cmake_minimum_required(VERSION 3.5)

project(zip LANGUAGES C)

file (GLOB SOURCES "*.h" "*.c")
add_library(${PROJECT_NAME}  STATIC ${SOURCES})
target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
