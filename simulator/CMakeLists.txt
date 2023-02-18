cmake_minimum_required(VERSION 3.16)
project(simulator)

set(CMAKE_CXX_STANDARD 17)

add_executable(simulator
	../share/utils.cpp
	src/encoding/baseframe.cpp
	src/encoding/jpegframe.cpp
	src/graphics/figures/figure.cpp
	src/graphics/figures/horizon.cpp
	src/graphics/figures/water.cpp
	src/graphics/figures/sol.cpp
	src/graphics/figures/antisubmarinefrigate.cpp
	src/graphics/figures/vessel.cpp \
	src/graphics/texture/texture.cpp
	src/graphics/texture/avi.cpp
	src/graphics/figureset.cpp
	src/graphics/uniformblock.cpp
	src/graphics/shader.cpp
	src/graphics/program.cpp
	src/graphics/scene.cpp
	src/graphics/screens/basescreen.cpp
	src/graphics/screens/glfwscreen.cpp
	src/server/s_socket.cpp
	src/server/baseprotocol.cpp
	src/server/flvprotocol.cpp
	src/server/s_poll.cpp
        src/service/baseservice.cpp
        src/service/glfwservice.cpp
        src/main.cpp
        )

target_link_libraries(simulator GL glfw GLEW SOIL jpeg pthread)
