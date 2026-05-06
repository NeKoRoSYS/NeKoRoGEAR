#include "DebugMenu.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include <iostream>

void DebugMenu::Init(SDL_Window* window, SDL_GLContext glContext) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = nullptr;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL3_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init("#version 330 core");
}

void DebugMenu::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

void DebugMenu::Begin() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void DebugMenu::Draw(float* clearColor) {
    ImGui::Begin("Debug Panel");
    ImGui::ColorEdit4("Clear Color", clearColor);
    ImGui::Checkbox("Spin Object", &spinObject);
    ImGui::End();
}

void DebugMenu::End() {
    ImGui::Render();
}

void DebugMenu::ProcessEvent(SDL_Event* event) {
    ImGui_ImplSDL3_ProcessEvent(event);
    if (ImGui::GetIO().WantCaptureMouse) pointerCaptured = true;
}

void DebugMenu::RenderDrawData() {
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}