cmake_minimum_required(VERSION 3.16)
project(videodevice)

set(CMAKE_CXX_STANDARD 14)

add_executable(videodevice
        src/service.cpp
        src/main.cpp
        )

target_include_directories(videodevice PRIVATE ${PROJECT_SOURCE_DIR}/../kformat/include  ${PROJECT_SOURCE_DIR}/../common)
link_directories(${CMAKE_BINARY_DIR}/kformat)
target_link_libraries(videodevice kformat jpeg)
