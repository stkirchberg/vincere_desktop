cmake_minimum_required(VERSION 3.10)
project(vincere_desktop)

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)
find_package(OpenGL REQUIRED)

include_directories(
    include
    extern/imgui
    extern/imgui/backends
    ${GLFW_INCLUDE_DIRS}
)

set(SOURCES
    src/main.cpp
    src/crypto_engine.cpp
    extern/imgui/imgui.cpp
    extern/imgui/imgui_draw.cpp
    extern/imgui/imgui_widgets.cpp
    extern/imgui/imgui_tables.cpp
    extern/imgui/imgui_demo.cpp
    extern/imgui/backends/imgui_impl_glfw.cpp
    extern/imgui/backends/imgui_impl_opengl3.cpp
)

add_executable(vincere_desktop ${SOURCES})

target_link_libraries(vincere_desktop 
    ${GLFW_LIBRARIES} 
    ${OPENGL_LIBRARIES} 
    dl
)