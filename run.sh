g++ src/main.cpp \
    extern/imgui/imgui*.cpp \
    extern/imgui/backends/imgui_impl_glfw.cpp \
    extern/imgui/backends/imgui_impl_opengl3.cpp \
    src/crypto_engine.cpp \
    -Iextern/imgui -Iextern/imgui/backends -Iinclude \
    -lglfw -lGL -ldl -lpthread -lcrypto \
    -o vincere-desktop

if [ $? -eq 0 ]; then
    ./vincere-desktop
else
    echo "Fehler beim Kompilieren!"
fi