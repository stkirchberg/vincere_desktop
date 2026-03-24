cmake_minimum_required(VERSION 3.10)
project(VincereDesktop)

set(CMAKE_CXX_STANDARD 17)

# Suche nach System-Paketen
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)
find_package(OpenSSL REQUIRED)

# Pfade für Header (Damit das Rot verschwindet)
include_directories(
    src
    extern/imgui
    extern/imgui/backends
    ${GLFW_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
)

add_executable(vincere_desktop 
    src/main.cpp 
    # Hier kommen später die ImGui .cpp Dateien dazu
)

target_link_libraries(vincere_desktop 
    ${GLFW_LIBRARIES} 
    OpenSSL::Crypto 
    GL
)