#pragma once
#include "GrandWindow.h"
#include <imgui.h>


class HashTable :
    public GrandWindow
{
    // private fields:
    ImGuiWindowFlags main_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGuiWindowFlags controls_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const ImGuiIO* io = &ImGui::GetIO();

    int cur_tool = 0;
    float speed = 1.f, curTime = 0;
    bool paused = false, camFollow = false, movingCam = false;
    ImVec2 camPos = { 0, 0 }, camTarget = { 0, 0 };

    // private methods:
    void controlsUpdate();
    void listUpdate();
    void followNode(int, int);

public:

    HashTable(std::string, int&, float&, bool&);
    ~HashTable();

    // public methods:
    void update();


};

