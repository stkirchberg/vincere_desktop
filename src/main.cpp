#include <iostream>
#include <vector>
#include <string>
#include <GLFW/glfw3.h>
#include "../include/crypto_engine.hpp"

void drawFilledRect(float x, float y, float w, float h, float r, float g, float b, int screenW, int screenH) {
    float nx = (2.0f * x) / screenW - 1.0f;
    float ny = 1.0f - (2.0f * y) / screenH;
    float nw = (2.0f * w) / screenW;
    float nh = (2.0f * h) / screenH;

    glBegin(GL_QUADS);
    glColor3f(r, g, b);
    glVertex2f(nx, ny);
    glVertex2f(nx + nw, ny);
    glVertex2f(nx + nw, ny - nh);
    glVertex2f(nx, ny - nh);
    glEnd();
}

void drawRectOutline(float x, float y, float w, float h, float r, float g, float b, int screenW, int screenH) {
    float nx = (2.0f * x) / screenW - 1.0f;
    float ny = 1.0f - (2.0f * y) / screenH;
    float nw = (2.0f * w) / screenW;
    float nh = (2.0f * h) / screenH;

    glBegin(GL_LINE_LOOP);
    glColor3f(r, g, b);
    glVertex2f(nx, ny);
    glVertex2f(nx + nw, ny);
    glVertex2f(nx + nw, ny - nh);
    glVertex2f(nx, ny - nh);
    glEnd();
}

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Vincere Desktop", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    Vincere::CryptoEngine crypto;
    if (!crypto.loadKeys("./keys/priv.bin")) {
        crypto.generateAndSave("./keys/priv.bin", "./keys/pub.bin");
    }

    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glClearColor(0.08f, 0.08f, 0.09f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        drawFilledRect(0, 0, 250, height, 0.1f, 0.1f, 0.12f, width, height);
        drawRectOutline(0, 0, 250, height, 0.3f, 0.3f, 0.3f, width, height);

        float btnY = 20;
        for(int i = 0; i < 3; ++i) {
            drawRectOutline(10, btnY, 230, 40, 0.5f, 0.5f, 0.5f, width, height);
            btnY += 50;
        }

        float chatX = 250;
        float chatW = width - 400;
        drawFilledRect(chatX, 0, chatW, 60, 0.12f, 0.12f, 0.15f, width, height);
        drawRectOutline(chatX, 0, chatW, 60, 0.3f, 0.3f, 0.3f, width, height);

        drawRectOutline(chatX + 20, 100, 400, 80, 0.6f, 0.6f, 0.6f, width, height);
        drawRectOutline(chatX + 100, 200, 400, 80, 0.6f, 0.6f, 0.6f, width, height);
        drawRectOutline(chatX + 20, 300, 400, 80, 0.6f, 0.6f, 0.6f, width, height);

        drawRectOutline(chatX + 10, height - 50, chatW - 20, 40, 0.4f, 0.4f, 0.4f, width, height);

        float infoX = chatX + chatW;
        drawFilledRect(infoX, 0, 150, height, 0.1f, 0.1f, 0.12f, width, height);
        drawRectOutline(infoX, 0, 150, height, 0.3f, 0.3f, 0.3f, width, height);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}