#pragma once
#include "GrandWindow.h"
#include <imgui.h>
#include <map>
#include <set>
#include <string>
class Tree :
    public GrandWindow
{
    // graph settings:
    #define EDGE_LENGTH 200.f
    #define VERTEX_RADIUS 30.f * zoomScale

    #define DEFAULT_VERT_COL ImGui::GetColorU32(IM_COL32(150, 150, 150, 255))

    
    struct Vertex {
        float x, y;
        float fx = 0, fy = 0;
        bool fixed = false;
        ImU32 color;
        Vertex() {
            x = (rand() % 10000) / 100.f;
            y = (rand() % 10000) / 100.f;
            color = DEFAULT_VERT_COL;
        }
    };

    ImGuiWindowFlags main_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGuiWindowFlags controls_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuiIO* io;

    ImVec2 camPos = { 0, 0 }, camTarget = { 0, 0 };
    int cur_tool = 0;
    bool movingCam = false;

    int n;
    std::string root;
    std::map<std::string, int> node_level;
    std::map<std::string, std::string> parent;
    std::map<std::string, Vertex> nodes;

     // private methods:

    ImU32 ContrastingColor(ImU32);
    void graphUpdate();
    float calcDist(float, float, float, float);
    void updateMenuBar();
    void controlsUpdate();
    void DrawEdge(ImDrawList*, const std::string, const std::string);

    public:

    Tree(std::string, int&, float&, bool&);
    ~Tree();

    // public methods:
    void update();


};

