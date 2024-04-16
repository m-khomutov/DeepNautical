cmake_minimum_required(VERSION 3.16)
project(kviewer)

set(CMAKE_CXX_STANDARD 14)


add_library(kviewer STATIC
        client/c_socket.cpp
        client/receiver.cpp
        decoder/basedecoder.cpp
        decoder/jpegdecoder.cpp
        viewer/baseviewer.cpp
        viewer/qviewer.cpp)

target_include_directories(kviewer PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/../common)
