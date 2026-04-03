#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "../include/crypto_engine.hpp"

struct Character {
    unsigned int TextureID;
    float SizeX, SizeY;
    float BearingX, BearingY;
    unsigned int Advance;
};

struct Message {
    std::string sender;
    std::string text;
    bool isSelf;
    float cachedWidth;
};

struct Chat {
    std::string name;
    std::string sub;
    std::vector<Message> messages;
    std::vector<std::string> members;
    std::string admin;
};

class VincereEngine {
public:
    std::map<char, Character> Characters;
    float sbW = 280.0f;
    float infoW = 200.0f;
    int sw, sh;
    int activeIdx = 0;
    bool isResizing = false;
    bool inputActive = false;
    std::vector<Chat> chats;
    std::string inputBuffer = "";
    Vincere::CryptoEngine crypto;

    VincereEngine() {
        Chat team = {"Vincere Team", "vincere-team", {}, {"neo", "paul", "ivan", "wolf", "kolya", "anton", "petro"}, "stk"};
        team.messages.push_back({"neo", "What we should do is having much more servers! Ours are overloaded.", false, 0});
        team.messages.push_back({"paul", "Agree. But there's one huge problem: We don't have enough money.", false, 0});
        team.messages.push_back({"stk", "So.. Don't talk. Plan and show your result.", true, 0});
        chats.push_back(team);
        chats.push_back({"neo", "direct-message", {{"neo", "Are the keys safe?", false, 0}}, {"stk", "neo"}, "stk"});
        chats.push_back({"Public Room", "public-chat", {}, {"everyone"}, "system"});
        chats.push_back({"Monero Fans", "xmr-crypto", {}, {"anon", "whale"}, "stk"});
        chats.push_back({"Dev Channel", "dev-vincere", {}, {"stk", "ivan"}, "ivan"});
        chats.push_back({"Security Hub", "sec-ops", {}, {"wolf", "salt"}, "wolf"});
        chats.push_back({"Network Stats", "bot-logs", {}, {"bot_1", "bot_2"}, "admin"});
        chats.push_back({"Hardware-Talk", "hw-general", {}, {"stk", "wolf"}, "wolf"});
        chats.push_back({"Coffee-Corner", "random", {}, {"everyone"}, "system"});
    }

    void initFonts() {
        FT_Library ft;
        FT_Init_FreeType(&ft);
        FT_Face face;
        FT_New_Face(ft, "/usr/share/fonts/TTF/DejaVuSans.ttf", 0, &face);
        FT_Set_Pixel_Sizes(face, 0, 14);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        for (unsigned char c = 0; c < 128; c++) {
            FT_Load_Char(face, c, FT_LOAD_RENDER);
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            unsigned char* buffer = new unsigned char[face->glyph->bitmap.width * face->glyph->bitmap.rows * 2];
            for(int i=0; i < face->glyph->bitmap.width * face->glyph->bitmap.rows; i++) {
                buffer[i*2] = 255;
                buffer[i*2+1] = face->glyph->bitmap.buffer[i];
            }
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, buffer);
            delete[] buffer;
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            Characters[c] = { texture, (float)face->glyph->bitmap.width, (float)face->glyph->bitmap.rows, (float)face->glyph->bitmap_left, (float)face->glyph->bitmap_top, (unsigned int)face->glyph->advance.x };
        }
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
        for (auto& c : chats) for (auto& m : c.messages) m.cachedWidth = calculateTextWidth(m.text);
    }

    float calculateTextWidth(const std::string& text) {
        float w = 0.0f;
        for (char c : text) w += (Characters[c].Advance >> 6);
        return w;
    }

    void drawRect(float x, float y, float w, float h, float r, float g, float b, float a = 1.0f, bool fill = true) {
        float nx = (2.0f * x) / sw - 1.0f, ny = 1.0f - (2.0f * y) / sh;
        float nw = (2.0f * w) / sw, nh = (2.0f * h) / sh;
        glColor4f(r, g, b, a);
        glBegin(fill ? GL_QUADS : GL_LINE_LOOP);
        glVertex2f(nx, ny); glVertex2f(nx + nw, ny); glVertex2f(nx + nw, ny - nh); glVertex2f(nx, ny - nh);
        glEnd();
    }

    void drawText(const std::string& text, float x, float y, float r, float g, float b, float scale = 1.0f) {
        glEnable(GL_TEXTURE_2D); glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor3f(r, g, b);
        float tX = (2.0f * x) / sw - 1.0f, tY = 1.0f - (2.0f * y) / sh;
        for (char c : text) {
            Character ch = Characters[c];
            float xpos = tX + (ch.BearingX / sw) * 2.0f * scale;
            float ypos = tY - ((ch.SizeY - ch.BearingY) / sh) * 2.0f * scale;
            float w = (ch.SizeX / sw) * 2.0f * scale, h = (ch.SizeY / sh) * 2.0f * scale;
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex2f(xpos, ypos + h); glTexCoord2f(1, 0); glVertex2f(xpos + w, ypos + h);
            glTexCoord2f(1, 1); glVertex2f(xpos + w, ypos); glTexCoord2f(0, 1); glVertex2f(xpos, ypos);
            glEnd();
            tX += (ch.Advance >> 6) * 2.0f / sw * scale;
        }
        glDisable(GL_TEXTURE_2D);
    }

    void update(GLFWwindow* window) {
        glfwGetFramebufferSize(window, &sw, &sh);
        double mx, my;
        glfwGetCursorPos(window, &mx, &my);
        bool overRes = (std::abs(mx - sbW) < 8.0);
        float chatW = sw - sbW - infoW;
        bool overIn = (mx > sbW + 35 && mx < sbW + 35 + chatW - 70 && my > sh - 75 && my < sh - 25);
        if (overRes || isResizing) glfwSetCursor(window, glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR));
        else if (overIn) glfwSetCursor(window, glfwCreateStandardCursor(GLFW_IBEAM_CURSOR));
        else glfwSetCursor(window, NULL);
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            if (overRes) isResizing = true;
            inputActive = overIn;
            if (mx < sbW && my > 200) {
                int clicked = (int)((my - 205) / 45);
                if (clicked >= 0 && clicked < (int)chats.size()) activeIdx = clicked;
            }
        } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) isResizing = false;
        if (isResizing) sbW = std::clamp((float)mx, 180.0f, 550.0f);
    }

    void handleKey(int key, int action) {
        if (!inputActive) return;
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            if (key == GLFW_KEY_BACKSPACE && !inputBuffer.empty()) inputBuffer.pop_back();
            else if (key == GLFW_KEY_ENTER && !inputBuffer.empty()) {
                Message m; m.sender = "stk"; m.text = inputBuffer; m.isSelf = true;
                m.cachedWidth = calculateTextWidth(inputBuffer);
                chats[activeIdx].messages.push_back(m);
                inputBuffer.clear();
            }
        }
    }

    void render() {
        glViewport(0, 0, sw, sh);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); glClear(GL_COLOR_BUFFER_BIT);
        float chatW = sw - sbW - infoW;
        drawRect(0, 0, sbW, sh, 0.98f, 0.98f, 0.99f);
        drawRect(sbW, 0, 1, sh, 0.1f, 0.1f, 0.1f);
        std::string btns[] = {"Join public room", "create or join room", "start or join chat"};
        for (int i = 0; i < 3; ++i) {
            drawRect(25, 25 + i * 45, sbW - 50, 35, 0.1f, 0.1f, 0.1f, 1.0f, false);
            drawText(btns[i], 40, 48 + i * 45, 0.1f, 0.1f, 0.1f);
        }
        drawText("Last conversations:", 25, 175, 0.2f, 0.2f, 0.2f);
        float cY = 205;
        for (int i = 0; i < (int)chats.size(); ++i) {
            if (i == activeIdx) drawRect(0, cY - 18, sbW, 45, 0.0f, 0.4f, 0.9f, 0.12f);
            drawRect(25, cY, 15, 15, 0.2f, 0.2f, 0.2f, 1.0f, false);
            drawText(chats[i].name, 55, cY + 12, 0.1f, 0.1f, 0.1f);
            cY += 45;
        }
        drawRect(sbW + 1, 0, chatW, 90, 1.0f, 1.0f, 1.0f);
        drawRect(sbW + 30, 20, 50, 50, 0.1f, 0.1f, 0.1f, 1.0f, false);
        drawText(chats[activeIdx].name, sbW + 100, 45, 0.1f, 0.1f, 0.1f, 1.4f);
        drawText(chats[activeIdx].sub, sbW + 100, 68, 0.4f, 0.4f, 0.4f);
        drawRect(sbW + 1, 90, chatW, 1, 0.1f, 0.1f, 0.1f);
        float mY = 135;
        for (auto& m : chats[activeIdx].messages) {
            float bW = std::max(m.cachedWidth + 35.0f, 80.0f), bH = 55;
            float mX = m.isSelf ? (sbW + chatW - bW - 40) : (sbW + 40);
            drawText(m.sender, mX, mY - 12, 0.3f, 0.3f, 0.3f, 0.85f);
            drawRect(mX, mY, bW, bH, 0.1f, 0.1f, 0.1f, 1.0f, false);
            drawText(m.text, mX + 15, mY + 34, 0.1f, 0.1f, 0.1f);
            mY += 95;
        }
        drawRect(sbW + 35, sh - 75, chatW - 70, 50, 0.1f, 0.1f, 0.1f, inputActive ? 1.0f : 0.6f, false);
        std::string out = (inputBuffer.empty() && !inputActive) ? "Write a message..." : inputBuffer;
        drawText(out, sbW + 55, sh - 42, 0.2f, 0.2f, 0.2f);
        float rX = sbW + chatW;
        drawRect(rX, 0, 1, sh, 0.1f, 0.1f, 0.1f);
        drawText("Admin:", rX + 25, 45, 0.1f, 0.1f, 0.1f, 1.25f);
        drawText(chats[activeIdx].admin, rX + 25, 75, 0.2f, 0.2f, 0.2f);
        drawText("Online:", rX + 25, 140, 0.1f, 0.1f, 0.1f, 1.25f);
        float memY = 175;
        for (auto& mem : chats[activeIdx].members) {
            drawRect(rX + 25, memY - 10, 10, 10, 0.4f, 0.8f, 0.4f);
            drawText(mem, rX + 45, memY, 0.2f, 0.2f, 0.2f);
            memY += 32;
        }
    }
};

VincereEngine* gE = nullptr;
void c_cb(GLFWwindow* w, unsigned int c) { if(gE->inputActive && c < 128) gE->inputBuffer += (char)c; }
void k_cb(GLFWwindow* w, int k, int s, int a, int m) { gE->handleKey(k, a); }

int main() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(1280, 800, "Vincere Native", NULL, NULL);
    glfwMakeContextCurrent(window);
    gE = new VincereEngine();
    gE->initFonts();
    glfwSetCharCallback(window, c_cb);
    glfwSetKeyCallback(window, k_cb);
    while (!glfwWindowShouldClose(window)) {
        gE->update(window);
        gE->render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}