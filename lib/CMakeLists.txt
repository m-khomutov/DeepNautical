cmake_minimum_required(VERSION 3.16)
project(lib)

set(CMAKE_CXX_STANDARD 14)


add_library(lib STATIC
    ../utils/utils.cpp
    base/screen.cpp)
#        service/socket.cpp
#        service/vdev.cpp
#        service/connection.cpp
#        service/loop.cpp
#        service/service.cpp
#        frame/frame.cpp
#        frame/jpeg.cpp
#        proto/proto.cpp
#        proto/flv.cpp
#        proto/mjpeg.cpp
#        proto/api.cpp)

target_include_directories(lib PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/../utils)
