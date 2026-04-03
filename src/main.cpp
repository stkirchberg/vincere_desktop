#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../include/crypto_engine.hpp"

void DrawChatBubble(const char* text, bool isSelf) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 textSize = ImGui::CalcTextSize(text);
    float padding = 10.0f;
    ImVec2 bubbleSize = ImVec2(textSize.x + padding * 2, textSize.y + padding * 2);

    if (isSelf) {
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - bubbleSize.x - 25.0f);
    } else {
        ImGui::SetCursorPosX(15.0f);
    }

    ImVec2 p = ImGui::GetCursorScreenPos();
    ImU32 bgColor = isSelf ? IM_COL32(43, 82, 120, 255) : IM_COL32(35, 35, 35, 255);
    
    drawList->AddRectFilled(p, ImVec2(p.x + bubbleSize.x, p.y + bubbleSize.y), bgColor, 12.0f);
    
    ImGui::SetCursorScreenPos(ImVec2(p.x + padding, p.y + padding));
    ImGui::TextUnformatted(text);
    
    ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + bubbleSize.y + 8.0f));
}

int main() {
    if (!glfwInit()) return 1;

    glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Vincere Desktop", NULL, NULL);
    if (!window) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.FrameRounding = 18.0f;
    style.ItemSpacing = ImVec2(10, 8);
    style.WindowPadding = ImVec2(0, 0);

    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.0f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.10f, 0.11f, 1.0f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.15f, 0.25f, 0.35f, 1.0f);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    Vincere::CryptoEngine crypto;
    if (!crypto.loadKeys("./keys/priv.bin")) {
        crypto.generateAndSave("./keys/priv.bin", "./keys/pub.bin");
    }

    float sidebarWidth = 280.0f;
    char inputBuf[1024] = "";
    static std::vector<std::pair<std::string, bool>> chatHistory;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("MainLayout", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
        
        ImGui::BeginChild("Sidebar", ImVec2(sidebarWidth, 0), true);
            ImGui::Spacing();
            ImGui::SetCursorPosX(20);
            ImGui::TextColored(ImVec4(0.4f, 0.6f, 0.9f, 1.0f), "VINCERE");
            ImGui::Separator();
            
            if (ImGui::Selectable("##Contact1", true, 0, ImVec2(0, 60))) {}
            ImGui::SameLine(15);
            ImGui::BeginGroup();
                ImGui::Text("Saskia Ritter");
                ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Latest message preview...");
            ImGui::EndGroup();
        ImGui::EndChild();

        ImGui::SameLine(0, 0);

        ImGui::Button("##splitter", ImVec2(2, -1));
        if (ImGui::IsItemActive()) sidebarWidth += ImGui::GetIO().MouseDelta.x;
        if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

        ImGui::SameLine(0, 0);

        ImGui::BeginGroup();
            ImGui::BeginChild("Header", ImVec2(0, 60), true);
                ImGui::SetCursorPos(ImVec2(20, 12));
                ImGui::Text("Saskia Ritter");
                ImGui::SetCursorPosX(20);
                ImGui::TextColored(ImVec4(0.4f, 0.7f, 0.4f, 1.0f), "online");
            ImGui::EndChild();

            ImGui::BeginChild("Messages", ImVec2(0, -70), false);
                for (auto& msg : chatHistory) {
                    DrawChatBubble(msg.first.c_str(), msg.second);
                }
            ImGui::EndChild();

            ImGui::SetCursorPosX(20);
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 40);
            if (ImGui::InputTextWithHint("##MsgInput", "Write a message...", inputBuf, 1024, ImGuiInputTextFlags_EnterReturnsTrue)) {
                if (strlen(inputBuf) > 0) {
                    chatHistory.push_back({inputBuf, true});
                    inputBuf[0] = '\0';
                }
            }
        ImGui::EndGroup();

        ImGui::End();

        ImGui::Render();
        glClearColor(0.08f, 0.08f, 0.09f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}