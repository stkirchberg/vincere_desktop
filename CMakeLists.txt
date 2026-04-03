cmake_minimum_required(VERSION 3.10)
project(vincere_desktop)

set(CMAKE_CXX_STANDARD 17)
set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Freetype REQUIRED)

include_directories(${FREETYPE_INCLUDE_DIRS} /usr/include/freetype2 include)

add_executable(vincere_desktop src/main.cpp src/crypto_engine.cpp)

target_link_libraries(vincere_desktop 
    glfw 
    OpenGL::GL 
    ${FREETYPE_LIBRARIES}
)