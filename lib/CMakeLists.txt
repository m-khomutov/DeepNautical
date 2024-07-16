cmake_minimum_required(VERSION 3.16)
project(nautical-dev)

set(CMAKE_CXX_STANDARD 14)

add_library(
    ${PROJECT_NAME}
    STATIC
    ../utils/utils.cpp
    base/screen.cpp
    base/service.cpp
    base/frame.cpp
    base/protocol.cpp
    service/tcpsock.cpp
    service/vdev.cpp
    service/connection.cpp
    service/loop.cpp
    frame/jpeg.cpp
    protocol/flv.cpp
    protocol/mjpeg.cpp
    protocol/api.cpp
    )

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/../utils
    )

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/lib
    )
