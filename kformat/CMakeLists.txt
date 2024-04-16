cmake_minimum_required(VERSION 3.16)
project(kformat)

set(CMAKE_CXX_STANDARD 14)


add_library(kformat STATIC
        ../common/utils.cpp
        screen/basescreen.cpp
        service/s_socket.cpp
        service/videodevice.cpp
        service/connection.cpp
        service/s_poll.cpp
        service/baseservice.cpp
        encoding/baseframe.cpp
        encoding/jpegframe.cpp
        protocol/baseprotocol.cpp
        protocol/flvprotocol.cpp
        protocol/httpapi.cpp)

target_include_directories(kformat PRIVATE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/../common)
