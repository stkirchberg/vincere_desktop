cmake_minimum_required(VERSION 3.10)
project(vincere_desktop)

set(CMAKE_CXX_STANDARD 17)

set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

set(SOURCES 
    src/main.cpp 
    src/crypto_engine.cpp
)

add_executable(vincere_desktop ${SOURCES})

target_link_libraries(vincere_desktop 
    glfw 
    OpenGL::GL
)