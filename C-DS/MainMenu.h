#pragma once
#include "GrandWindow.h"
#include <imgui.h>


class MainMenu :
    public GrandWindow
{
private:
    // private fields:
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    // private methods:
    void updateMenuBar();

public:

    MainMenu(std::string, int&, float&, bool&);
    ~MainMenu();

    // public methods:
    void update();

};

