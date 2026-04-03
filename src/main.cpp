#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <GLFW/glfw3.h>
#include "../include/crypto_engine.hpp"

struct Message {
    std::string sender;
    float width;
    float height;
    bool isSelf;
    float r, g, b;
};

struct Chat {
    std::string name;
    std::string subtitle;
    std::vector<Message> messages;
    std::vector<std::string> members;
    float scrollOffset;
};

class VincereApp {
public:
    int screenW, screenH;
    int activeChatIdx = 0;
    std::vector<Chat> chatList;
    float sidebarWidth = 280.0f;
    float infoWidth = 200.0f;
    bool isDraggingSidebar = false;
    double lastKeyTime = 0;

    VincereApp() {
        srand(static_cast<unsigned int>(time(0)));
        initData();
    }

    void initData() {
        Chat team;
        team.name = "Vincere Team";
        team.subtitle = "vincere-team";
        team.scrollOffset = 0;
        team.members = {"stk", "neo", "paul", "ivan", "wolf", "salt", "anton", "petra"};
        team.messages.push_back({"neo", 320.0f, 60.0f, false, 0.92f, 0.92f, 0.94f});
        team.messages.push_back({"paul", 280.0f, 60.0f, false, 0.92f, 0.92f, 0.94f});
        team.messages.push_back({"ivan", 380.0f, 80.0f, false, 0.92f, 0.92f, 0.94f});
        
        Chat privateNeo;
        privateNeo.name = "neo";
        privateNeo.subtitle = "direct message";
        privateNeo.scrollOffset = 0;
        privateNeo.members = {"stk", "neo"};
        
        chatList.push_back(team);
        chatList.push_back(privateNeo);
        chatList.push_back({"Public Room", "global", {}, {"all_users"}, 0});
        chatList.push_back({"Monero Fans", "xmr-community", {}, {"crypto_maniac"}, 0});
        chatList.push_back({"Development", "internal", {}, {"stk", "dev_bot"}, 0});
    }

    void drawQuad(float x, float y, float w, float h, float r, float g, float b, bool filled = true) {
        float nx = (2.0f * x) / screenW - 1.0f;
        float ny = 1.0f - (2.0f * y) / screenH;
        float nw = (2.0f * w) / screenW;
        float nh = (2.0f * h) / screenH;

        if (filled) glBegin(GL_QUADS);
        else glBegin(GL_LINE_LOOP);
        
        glColor3f(r, g, b);
        glVertex2f(nx, ny);
        glVertex2f(nx + nw, ny);
        glVertex2f(nx + nw, ny - nh);
        glVertex2f(nx, ny - nh);
        glEnd();
    }

    void handleEvents(GLFWwindow* window) {
        double currentTime = glfwGetTime();
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            if (std::abs(mouseX - sidebarWidth) < 10.0) isDraggingSidebar = true;
        } else {
            isDraggingSidebar = false;
        }

        if (isDraggingSidebar) {
            sidebarWidth = static_cast<float>(mouseX);
            if (sidebarWidth < 150.0f) sidebarWidth = 150.0f;
            if (sidebarWidth > 500.0f) sidebarWidth = 500.0f;
        }

        if (currentTime - lastKeyTime > 0.15) {
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
                activeChatIdx = (activeChatIdx + 1) % chatList.size();
                lastKeyTime = currentTime;
            }
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
                activeChatIdx = (activeChatIdx - 1 + (int)chatList.size()) % (int)chatList.size();
                lastKeyTime = currentTime;
            }
            if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
                float rndW = 150.0f + (rand() % 300);
                chatList[activeChatIdx].messages.push_back({"stk", rndW, 50.0f, true, 0.0f, 0.48f, 1.0f});
                lastKeyTime = currentTime;
            }
        }
    }

    void render(GLFWwindow* window) {
        glfwGetFramebufferSize(window, &screenW, &screenH);
        glViewport(0, 0, screenW, screenH);
        glClearColor(0.98f, 0.98f, 0.99f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float centerW = screenW - sidebarWidth - infoWidth;

        drawQuad(0, 0, sidebarWidth, screenH, 1.0f, 1.0f, 1.0f);
        drawQuad(sidebarWidth, 0, 1, screenH, 0.88f, 0.88f, 0.90f);

        float btnY = 20.0f;
        for (int i = 0; i < 3; ++i) {
            drawQuad(20, btnY, sidebarWidth - 40, 45, 0.96f, 0.96f, 0.98f);
            drawQuad(20, btnY, sidebarWidth - 40, 45, 0.80f, 0.80f, 0.85f, false);
            btnY += 55.0f;
        }

        btnY += 25.0f;
        for (int i = 0; i < chatList.size(); ++i) {
            if (i == activeChatIdx) {
                drawQuad(0, btnY, sidebarWidth, 65, 0.93f, 0.96f, 1.0f);
                drawQuad(0, btnY, 4, 65, 0.0f, 0.5f, 1.0f);
            }
            drawQuad(15, btnY + 12, 40, 40, 0.85f, 0.85f, 0.88f);
            drawQuad(15, btnY + 12, 40, 40, 0.75f, 0.75f, 0.75f, false);
            btnY += 66.0f;
        }

        float headX = sidebarWidth + 1;
        drawQuad(headX, 0, centerW, 70, 1.0f, 1.0f, 1.0f);
        drawQuad(headX, 70, centerW, 1, 0.88f, 0.88f, 0.90f);
        drawQuad(headX + 20, 15, 40, 40, 0.9f, 0.9f, 0.9f);

        float msgY = 100.0f;
        for (auto& m : chatList[activeChatIdx].messages) {
            float msgX = m.isSelf ? (sidebarWidth + centerW - m.width - 20) : (sidebarWidth + 20);
            drawQuad(msgX, msgY, m.width, m.height, m.r, m.g, m.b);
            drawQuad(msgX, msgY, m.width, m.height, 0.82f, 0.82f, 0.85f, false);
            msgY += m.height + 15.0f;
        }

        float inputY = screenH - 70.0f;
        drawQuad(sidebarWidth + 1, inputY, centerW, 70, 1.0f, 1.0f, 1.0f);
        drawQuad(sidebarWidth + 20, inputY + 15, centerW - 120, 40, 0.97f, 0.97f, 0.98f);
        drawQuad(sidebarWidth + 20, inputY + 15, centerW - 120, 40, 0.75f, 0.75f, 0.75f, false);
        drawQuad(sidebarWidth + centerW - 80, inputY + 15, 60, 40, 0.0f, 0.48f, 1.0f);

        float rX = sidebarWidth + centerW;
        drawQuad(rX, 0, infoWidth, screenH, 0.98f, 0.98f, 1.0f);
        drawQuad(rX, 0, 1, screenH, 0.88f, 0.88f, 0.90f);

        float memY = 85.0f;
        for (int i = 0; i < chatList[activeChatIdx].members.size(); ++i) {
            drawQuad(rX + 20, memY, 10, 10, 0.45f, 0.85f, 0.45f);
            memY += 28.0f;
        }
    }
};

int main() {
    if (!glfwInit()) return -1;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    
    GLFWwindow* window = glfwCreateWindow(1280, 800, "Vincere Desktop", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    Vincere::CryptoEngine crypto;
    VincereApp app;

    while (!glfwWindowShouldClose(window)) {
        app.handleEvents(window);
        app.render(window);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}