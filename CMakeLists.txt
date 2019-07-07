cmake_minimum_required(VERSION 3.13)  # CMake version check
project(runner)                       # Create project "runner"

set(CMAKE_CXX_STANDARD 17)            # Enable c++14 standard

include_directories("${PROJECT_SOURCE_DIR}/inc/")
# Add executable target with source files listed in SOURCE_FILES variable
add_executable(runner filewatcher/index.h core/index.h core/index.cpp filewatcher/index.cpp index.cpp)