cmake_minimum_required(VERSION 3.16)
project(simulator)

set(CMAKE_CXX_STANDARD 17)

add_executable(simulator
        src/graphics/figures/sol.cpp
        src/graphics/figures/water.cpp
        src/graphics/figures/horizon.cpp
        src/graphics/figures/antisubmarinefrigate.cpp
        src/graphics/figures/vessel.cpp
        src/graphics/figures/sparklets.cpp
        src/graphics/figures/figure.cpp
        src/graphics/figures/specification.cpp
        src/graphics/visitor.cpp
        src/graphics/figureset.cpp
        src/graphics/program.cpp
        src/graphics/scene.cpp
        src/graphics/blender.cpp
        src/graphics/screens/glscreen.cpp
        src/graphics/shader.cpp
        src/graphics/texture/texture.cpp
        src/graphics/texture/avi.cpp
        src/graphics/uniformblock.cpp
        src/service/glfwservice.cpp
        src/main.cpp
        )

target_include_directories(simulator PRIVATE ${PROJECT_SOURCE_DIR}/..)
link_directories(${CMAKE_BINARY_DIR}/kformat)
target_link_libraries(simulator kformat GL glfw GLEW GLU jpeg pthread)
