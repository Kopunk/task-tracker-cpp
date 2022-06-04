cmake_minimum_required(VERSION 3.13)

project(task-tracker++)
add_executable(track src/main.cpp)

# add_compile_definitions(ENABLE_LOG)

include_directories(include)

set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/bin/)
