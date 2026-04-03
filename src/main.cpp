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
};

struct Chat {
    std::string name;
    std::vector<Message> messages;
    std::vector<std::string> members;
};

class VincereApp {
public:
    std::map<char, Character> Characters;
    float sbW = 260.0f;
    float infoW = 180.0f;
    int activeChat = 0;
    bool resizing = false;
    int sw, sh;
    std::vector<Chat> chats;
    double lastInput = 0;

    VincereApp() {
        setupData();
    }

    void setupData() {
        Chat team = {"Vincere Team", {}, {"stk", "neo", "paul", "ivan", "wolf", "salt", "anton", "petra"}};
        team.messages.push_back({"neo", "What we should do is having much more servers!", false});
        team.messages.push_back({"stk", "Plan and show your result.", true});
        team.messages.push_back({"paul", "The database is ready for the migration.", false});
        team.messages.push_back({"ivan", "I will check the logs tonight.", false});
        chats.push_back(team);
        
        Chat privateNeo = {"neo", {}, {"stk", "neo"}};
        privateNeo.messages.push_back({"neo", "Are the keys safe?", false});
        privateNeo.messages.push_back({"stk", "Yes, AES-256 is active.", true});
        chats.push_back(privateNeo);

        chats.push_back({"Public Room", {{"System", "Welcome to the hub.", false}}, {"all"}});
        chats.push_back({"Monero Fans", {}, {"xmr_king", "privacy_dev"}});
        chats.push_back({"Dev Channel", {{"ivan", "Build is failing on Arch.", false}}, {"ivan", "stk"}});
        chats.push_back({"Security", {{"Admin", "Patching now.", false}}, {"admin_01"}});
        chats.push_back({"Offtopic", {}, {"everyone"}});
        chats.push_back({"Archive", {}, {"system"}});
    }

    void initFonts() {
        FT_Library ft;
        if (FT_Init_FreeType(&ft)) return;
        FT_Face face;
        if (FT_New_Face(ft, "/usr/share/fonts/TTF/DejaVuSans.ttf", 0, &face)) return;
        FT_Set_Pixel_Sizes(face, 0, 13);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        for (unsigned char c = 0; c < 128; c++) {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            Character character = { texture, (float)face->glyph->bitmap.width, (float)face->glyph->bitmap.rows, (float)face->glyph->bitmap_left, (float)face->glyph->bitmap_top, (unsigned int)face->glyph->advance.x };
            Characters.insert(std::pair<char, Character>(c, character));
        }
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }

    void drawText(std::string text, float x, float y, float r, float g, float b) {
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor3f(r, g, b);
        float tempX = (2.0f * x) / sw - 1.0f;
        float tempY = 1.0f - (2.0f * y) / sh;
        for (char c : text) {
            Character ch = Characters[c];
            float xpos = tempX + (ch.BearingX / sw) * 2.0f;
            float ypos = tempY - ((ch.SizeY - ch.BearingY) / sh) * 2.0f;
            float w = (ch.SizeX / sw) * 2.0f;
            float h = (ch.SizeY / sh) * 2.0f;
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex2f(xpos, ypos + h);
            glTexCoord2f(1, 0); glVertex2f(xpos + w, ypos + h);
            glTexCoord2f(1, 1); glVertex2f(xpos + w, ypos);
            glTexCoord2f(0, 1); glVertex2f(xpos, ypos);
            glEnd();
            tempX += (ch.Advance >> 6) * 2.0f / sw;
        }
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
    }

    void drawRect(float x, float y, float w, float h, float r, float g, float b, bool fill = true) {
        float nx = (2.0f * x) / sw - 1.0f;
        float ny = 1.0f - (2.0f * y) / sh;
        float nw = (2.0f * w) / sw;
        float nh = (2.0f * h) / sh;
        glBegin(fill ? GL_QUADS : GL_LINE_LOOP);
        glColor3f(r, g, b);
        glVertex2f(nx, ny); glVertex2f(nx + nw, ny);
        glVertex2f(nx + nw, ny - nh); glVertex2f(nx, ny - nh);
        glEnd();
    }

    void update(GLFWwindow* window) {
        glfwGetFramebufferSize(window, &sw, &sh);
        double mx, my;
        glfwGetCursorPos(window, &mx, &my);
        bool over = (std::abs(mx - sbW) < 10.0);
        if (over || resizing) {
            glfwSetCursor(window, glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR));
        } else {
            glfwSetCursor(window, NULL);
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && over) resizing = true;
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) resizing = false;
        if (resizing) sbW = std::clamp((float)mx, 150.0f, 600.0f);
        double now = glfwGetTime();
        if (now - lastInput > 0.2) {
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
                activeChat = (activeChat + 1) % chats.size();
                lastInput = now;
            }
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
                activeChat = (activeChat - 1 + (int)chats.size()) % (int)chats.size();
                lastInput = now;
            }
        }
    }

    void render() {
        glViewport(0, 0, sw, sh);
        glClearColor(0.98f, 0.98f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        float chatW = sw - sbW - infoW;
        drawRect(0, 0, sbW, sh, 1.0f, 1.0f, 1.0f);
        drawRect(sbW, 0, 1, sh, 0.85f, 0.85f, 0.85f);
        for(int i = 0; i < 3; ++i) {
            drawRect(20, 20 + i * 50, sbW - 40, 40, 0.95f, 0.96f, 0.98f);
            drawRect(20, 20 + i * 50, sbW - 40, 40, 0.82f, 0.82f, 0.85f, false);
            drawText("Action", 35, 45 + i * 50, 0.5f, 0.5f, 0.6f);
        }
        float cY = 190;
        for(int i = 0; i < (int)chats.size(); ++i) {
            if(i == activeChat) {
                drawRect(0, cY, sbW, 60, 0.92f, 0.95f, 1.0f);
                drawRect(0, cY, 5, 60, 0.0f, 0.5f, 1.0f);
            }
            drawRect(15, cY + 10, 40, 40, 0.88f, 0.88f, 0.92f);
            drawText(chats[i].name, 65, cY + 35, 0.2f, 0.2f, 0.2f);
            cY += 62;
        }
        drawRect(sbW + 1, 0, chatW, 65, 1.0f, 1.0f, 1.0f);
        drawRect(sbW + 1, 65, chatW, 1, 0.85f, 0.85f, 0.85f);
        drawText(chats[activeChat].name, sbW + 30, 40, 0.1f, 0.1f, 0.1f);
        float mY = 100;
        for(auto& m : chats[activeChat].messages) {
            float mX = m.isSelf ? (sbW + chatW - 320) : (sbW + 25);
            drawRect(mX, mY, 300, 55, m.isSelf ? 0.0f : 0.94f, m.isSelf ? 0.52f : 0.94f, m.isSelf ? 1.0f : 0.98f);
            drawRect(mX, mY, 300, 55, 0.85f, 0.85f, 0.88f, false);
            drawText(m.text, mX + 15, mY + 32, m.isSelf ? 1.0f : 0.2f, m.isSelf ? 1.0f : 0.2f, m.isSelf ? 1.0f : 0.2f);
            mY += 70;
        }
        drawRect(sbW + 20, sh - 65, chatW - 40, 45, 1.0f, 1.0f, 1.0f);
        drawRect(sbW + 20, sh - 65, chatW - 40, 45, 0.8f, 0.8f, 0.8f, false);
        drawText("Write a message...", sbW + 40, sh - 38, 0.6f, 0.6f, 0.6f);
        float rX = sbW + chatW;
        drawRect(rX, 0, infoW, sh, 0.99f, 0.99f, 1.0f);
        drawRect(rX, 0, 1, sh, 0.85f, 0.85f, 0.85f);
        drawText("PARTICIPANTS", rX + 20, 40, 0.4f, 0.4f, 0.5f);
        float memY = 85;
        for(auto& mem : chats[activeChat].members) {
            drawRect(rX + 20, memY, 10, 10, 0.45f, 0.85f, 0.45f);
            drawText(mem, rX + 40, memY + 10, 0.3f, 0.3f, 0.3f);
            memY += 30;
        }
    }
};

int main() {
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(1280, 800, "Vincere Desktop", NULL, NULL);
    if (!window) return -1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    Vincere::CryptoEngine crypto;
    VincereApp app;
    app.initFonts();
    while (!glfwWindowShouldClose(window)) {
        app.update(window);
        app.render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}