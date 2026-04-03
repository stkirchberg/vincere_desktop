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
    bool isSelf;
    float r, g, b;
};

struct Chat {
    std::string name;
    std::string subtitle;
    std::vector<Message> messages;
    std::vector<std::string> members;
};

class VincereEngine {
public:
    int w, h;
    int activeIdx = 0;
    std::vector<Chat> chatList;
    bool keyLock = false;

    VincereEngine() {
        srand(time(0));
        setupData();
    }

    void setupData() {
        Chat team;
        team.name = "Vincere Team";
        team.subtitle = "vincere-team";
        team.members = {"stk", "neo", "paul", "ivan", "wolf", "salt", "anton", "petra"};
        team.messages.push_back({"neo", 300.0f, false, 0.9f, 0.9f, 0.95f});
        team.messages.push_back({"paul", 250.0f, false, 0.9f, 0.9f, 0.95f});
        
        Chat neo;
        neo.name = "neo";
        neo.subtitle = "private chat";
        neo.members = {"stk", "neo"};
        
        chatList.push_back(team);
        chatList.push_back(neo);
        chatList.push_back({"Public Room", "global", {}, {"everyone"}});
        chatList.push_back({"Monero Fans", "xmr-community", {}, {"crypto-maniac"}});
    }

    void drawBox(float x, float y, float width, float height, float r, float g, float b, bool filled = true) {
        float nx = (2.0f * x) / w - 1.0f;
        float ny = 1.0f - (2.0f * y) / h;
        float nw = (2.0f * width) / w;
        float nh = (2.0f * height) / h;

        if (filled) glBegin(GL_QUADS);
        else glBegin(GL_LINE_LOOP);
        
        glColor3f(r, g, b);
        glVertex2f(nx, ny);
        glVertex2f(nx + nw, ny);
        glVertex2f(nx + nw, ny - nh);
        glVertex2f(nx, ny - nh);
        glEnd();
    }

    void render(GLFWwindow* window) {
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glClearColor(0.98f, 0.98f, 0.99f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float sideW = 280.0f;
        float infoW = 180.0f;
        float centerW = w - sideW - infoW;

        drawBox(0, 0, sideW, h, 1.0f, 1.0f, 1.0f);
        drawBox(sideW, 0, 1, h, 0.85f, 0.85f, 0.85f);

        float btnY = 20.0f;
        for(int i = 0; i < 3; ++i) {
            drawBox(20, btnY, sideW - 40, 45, 0.95f, 0.95f, 0.97f);
            drawBox(20, btnY, sideW - 40, 45, 0.7f, 0.7f, 0.7f, false);
            btnY += 55.0f;
        }

        btnY += 20.0f;
        for(int i = 0; i < chatList.size(); ++i) {
            if(i == activeIdx) {
                drawBox(0, btnY, sideW, 60, 0.92f, 0.95f, 1.0f);
                drawBox(0, btnY, 4, 60, 0.0f, 0.5f, 1.0f);
            }
            drawBox(15, btnY + 15, 30, 30, 0.8f, 0.8f, 0.85f);
            btnY += 61.0f;
        }

        drawBox(sideW + 1, 0, centerW, 65, 1.0f, 1.0f, 1.0f);
        drawBox(sideW + 1, 65, centerW, 1, 0.85f, 0.85f, 0.85f);

        float mY = 100.0f;
        for(auto& m : chatList[activeIdx].messages) {
            float mX = m.isSelf ? (sideW + centerW - m.width - 20) : (sideW + 20);
            drawBox(mX, mY, m.width, 50, m.r, m.g, m.b);
            drawBox(mX, mY, m.width, 50, 0.8f, 0.8f, 0.8f, false);
            mY += 65.0f;
        }

        drawBox(sideW + 20, h - 60, centerW - 40, 45, 1.0f, 1.0f, 1.0f);
        drawBox(sideW + 20, h - 60, centerW - 40, 45, 0.7f, 0.7f, 0.7f, false);

        float rX = sideW + centerW;
        drawBox(rX, 0, infoW, h, 0.98f, 0.98f, 1.0f);
        drawBox(rX, 0, 1, h, 0.85f, 0.85f, 0.85f);

        float memY = 80.0f;
        for(int i = 0; i < chatList[activeIdx].members.size(); ++i) {
            drawBox(rX + 20, memY, 8, 8, 0.4f, 0.8f, 0.4f);
            memY += 25.0f;
        }
    }

    void update(GLFWwindow* window) {
        if(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && !keyLock) {
            float len = 100.0f + (rand() % 250);
            chatList[activeIdx].messages.push_back({"stk", len, true, 0.0f, 0.5f, 1.0f});
            keyLock = true;
        }
        if(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE) keyLock = false;

        if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            activeIdx = (activeIdx + 1) % chatList.size();
            glfwWaitEventsTimeout(0.15);
        }
        if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            activeIdx = (activeIdx - 1 + chatList.size()) % chatList.size();
            glfwWaitEventsTimeout(0.15);
        }
    }
};

int main() {
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Vincere Native", NULL, NULL);
    if (!window) return -1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    Vincere::CryptoEngine crypto;
    VincereEngine engine;

    while (!glfwWindowShouldClose(window)) {
        engine.update(window);
        engine.render(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}