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
    std::vector<Message> messages;
    std::vector<std::string> members;
};

class VincereEngine {
public:
    std::map<char, Character> Characters;
    float sbW = 260.0f;
    float infoW = 180.0f;
    int sw, sh;
    int activeIdx = 0;
    bool isResizing = false;
    double lastInput = 0;
    std::vector<Chat> chats;

    VincereEngine() { setupData(); }

    void setupData() {
        Chat team = {"Vincere Team", {}, {"stk", "neo", "paul", "ivan", "wolf", "salt", "anton", "petra"}};
        team.messages.push_back({"neo", "What we should do is having much more servers!", false, 0});
        team.messages.push_back({"paul", "Agree. But there's one huge problem: We have no money.", false, 0});
        team.messages.push_back({"stk", "Plan and show your result.", true, 0});
        chats.push_back(team);
        chats.push_back({"neo", {{"neo", "Are the keys safe?", false, 0}}, {"stk", "neo"}});
        chats.push_back({"Public Room", {}, {"everyone"}});
        chats.push_back({"Monero Fans", {}, {"xmr_king", "anon"}});
    }

    void initFonts() {
        FT_Library ft;
        if (FT_Init_FreeType(&ft)) return;
        FT_Face face;
        const char* fontPath = "/usr/share/fonts/TTF/DejaVuSans.ttf";
        if (FT_New_Face(ft, fontPath, 0, &face)) return;
        
        FT_Set_Pixel_Sizes(face, 0, 14);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        
        for (unsigned char c = 0; c < 128; c++) {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;
            
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

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            Character character = { texture, (float)face->glyph->bitmap.width, (float)face->glyph->bitmap.rows, (float)face->glyph->bitmap_left, (float)face->glyph->bitmap_top, (unsigned int)face->glyph->advance.x };
            Characters.insert(std::pair<char, Character>(c, character));
        }
        FT_Done_Face(face);
        FT_Done_FreeType(ft);

        for (auto& chat : chats) {
            for (auto& msg : chat.messages) msg.cachedWidth = calculateTextWidth(msg.text);
        }
    }

    float calculateTextWidth(const std::string& text) {
        float width = 0.0f;
        for (char c : text) width += (Characters[c].Advance >> 6);
        return width;
    }

    void drawRect(float x, float y, float w, float h, float r, float g, float b, float a = 1.0f, bool fill = true) {
        float nx = (2.0f * x) / sw - 1.0f;
        float ny = 1.0f - (2.0f * y) / sh;
        float nw = (2.0f * w) / sw;
        float nh = (2.0f * h) / sh;
        glColor4f(r, g, b, a);
        glBegin(fill ? GL_QUADS : GL_LINE_LOOP);
        glVertex2f(nx, ny); glVertex2f(nx + nw, ny);
        glVertex2f(nx + nw, ny - nh); glVertex2f(nx, ny - nh);
        glEnd();
    }

    void drawText(const std::string& text, float x, float y, float r, float g, float b) {
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor3f(r, g, b);
        float tX = (2.0f * x) / sw - 1.0f;
        float tY = 1.0f - (2.0f * y) / sh;
        for (char c : text) {
            Character ch = Characters[c];
            float xpos = tX + (ch.BearingX / sw) * 2.0f;
            float ypos = tY - ((ch.SizeY - ch.BearingY) / sh) * 2.0f;
            float w = (ch.SizeX / sw) * 2.0f;
            float h = (ch.SizeY / sh) * 2.0f;
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex2f(xpos, ypos + h);
            glTexCoord2f(1, 0); glVertex2f(xpos + w, ypos + h);
            glTexCoord2f(1, 1); glVertex2f(xpos + w, ypos);
            glTexCoord2f(0, 1); glVertex2f(xpos, ypos);
            glEnd();
            tX += (ch.Advance >> 6) * 2.0f / sw;
        }
        glDisable(GL_TEXTURE_2D);
    }

    void update(GLFWwindow* window) {
        glfwGetFramebufferSize(window, &sw, &sh);
        double mx, my;
        glfwGetCursorPos(window, &mx, &my);
        bool over = (std::abs(mx - sbW) < 10.0);
        if (over || isResizing) glfwSetCursor(window, glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR));
        else glfwSetCursor(window, NULL);
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && over) isResizing = true;
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) isResizing = false;
        if (isResizing) sbW = std::clamp((float)mx, 150.0f, 500.0f);
        double now = glfwGetTime();
        if (now - lastInput > 0.15) {
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
                activeIdx = (activeIdx + 1) % chats.size();
                lastInput = now;
            }
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
                activeIdx = (activeIdx - 1 + (int)chats.size()) % (int)chats.size();
                lastInput = now;
            }
        }
    }

    void render() {
        glViewport(0, 0, sw, sh);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        float chatW = sw - sbW - infoW;

        drawRect(0, 0, sbW, sh, 0.96f, 0.96f, 0.98f);
        drawRect(sbW, 0, 1, sh, 0.88f, 0.88f, 0.90f);

        for (int i = 0; i < 3; ++i) {
            drawRect(20, 20 + i * 45, sbW - 40, 35, 0.4f, 0.4f, 0.45f, 1.0f, false);
            drawText("Action Item", 35, 42 + i * 45, 0.4f, 0.4f, 0.45f);
        }

        float cY = 180;
        for (int i = 0; i < (int)chats.size(); ++i) {
            if (i == activeIdx) drawRect(0, cY, sbW, 55, 0.0f, 0.4f, 0.9f, 0.08f);
            drawText(chats[i].name, 50, cY + 32, 0.2f, 0.2f, 0.2f);
            cY += 56;
        }

        drawRect(sbW + 1, 0, chatW, 60, 1.0f, 1.0f, 1.0f);
        drawRect(sbW + 1, 60, chatW, 1, 0.88f, 0.88f, 0.90f);
        drawText(chats[activeIdx].name, sbW + 30, 38, 0.1f, 0.1f, 0.1f);

        float mY = 90;
        for (auto& m : chats[activeIdx].messages) {
            float bW = m.cachedWidth + 30;
            float bH = 45;
            float mX = m.isSelf ? (sbW + chatW - bW - 25) : (sbW + 25);
            drawRect(mX, mY, bW, bH, 0.88f, 0.88f, 0.92f, 1.0f, false);
            drawText(m.text, mX + 15, mY + 28, 0.15f, 0.15f, 0.15f);
            mY += bH + 15;
        }

        drawRect(sbW + 20, sh - 60, chatW - 40, 40, 0.88f, 0.88f, 0.92f, 1.0f, false);
        drawText("Write a message...", sbW + 35, sh - 35, 0.6f, 0.6f, 0.6f);

        float rX = sbW + chatW;
        drawRect(rX, 0, infoW, sh, 0.99f, 0.99f, 1.0f);
        drawRect(rX, 0, 1, sh, 0.88f, 0.88f, 0.90f);
        drawText("PARTICIPANTS", rX + 20, 38, 0.5f, 0.5f, 0.6f);
        float memY = 80;
        for (auto& mem : chats[activeIdx].members) {
            drawRect(rX + 20, memY, 10, 10, 0.5f, 0.8f, 0.5f);
            drawText(mem, rX + 40, memY + 10, 0.3f, 0.3f, 0.3f);
            memY += 28;
        }
    }
};

int main() {
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(1280, 800, "Vincere Native", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    VincereEngine engine;
    engine.initFonts();
    while (!glfwWindowShouldClose(window)) {
        engine.update(window);
        engine.render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}