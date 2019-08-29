cmake_minimum_required(VERSION 3.13)  # CMake version check
project(runner)

include_directories(${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)            # Enable c++17 standard
set(SOURCE_FILES src/filewatcher/index.cpp src/core/index.cpp src/constants/index.cpp src/cli/index.cpp index.cpp src/table/index.cpp src/table/index.h)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} jsoncpp)
