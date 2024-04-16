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

find_package(Qt5Widgets)

target_include_directories(kviewer PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/../common ${Qt5Widgets_INCLUDES})
target_link_libraries(kviewer ${Qt5Widgets_LIBRARIES})
