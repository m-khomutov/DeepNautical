cmake_minimum_required(VERSION 3.17)
project(cometviewer)

set(CMAKE_CXX_STANDARD 17)

include(FindPkgConfig)

pkg_check_modules(GTK REQUIRED gtk+-3.0)
#pkg_search_module(GTK gtk+-3.0)
#if (NOT GTK_FOUND)
#  pkg_search_module(GTK gtk+-2.0)
#  if (GTK_FOUND)
#    message(STATUS "Using GTK 2")
#    pkg_search_module(GDK gdk-2.0)
#  endif ()
#else ()
#  message(STATUS "Using GTK 3")
#  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-deprecated-declarations")
#  pkg_search_module(GDK gdk-3.0)
#endif ()

include_directories(${GTK_INCLUDE_DIRS})
add_executable(cometviewer
	src/decoder/basedecoder.cpp
	src/decoder/jpegdecoder.cpp
	src/receiver/c_socket.cpp
	src/receiver/receiver.cpp
        src/viewer.cpp
        src/main.cpp
        )

target_link_libraries(cometviewer jpeg pthread ${GTK_LIBRARIES})
