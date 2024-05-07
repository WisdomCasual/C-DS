#pragma once
#include "GrandWindow.h"
#include <imgui.h>
#include <map>
#include <set>
#include <string>

class DSU :
    public GrandWindow
{

private:
    // graph settings:
    #define EDGE_LENGTH 200.f
    #define VERTEX_RADIUS 30.f * zoomScale

    #define FIXED_NODE_COLOR ImGui::GetColorU32(IM_COL32(40, 40, 40, 255))

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

    // private fields:

    ImGuiWindowFlags main_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGuiWindowFlags controls_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuiIO* io;

    ImVec2 camPos = { 0, 0 }, camTarget = { 0, 0 };
    int cur_tool = 0;
    bool movingCam = false, leftClickPressed = false;

    int n;
    std::map<std::string, int> rank, size;
    std::map<std::string, std::string> parent;
    std::map<std::string, Vertex> nodes;
    char add_node_text[15];

    std::string dragging;

    // private methods:

    ImU32 ContrastingColor(ImU32);
    void graphUpdate();
    float calcDist(float, float, float, float);
    void updateMenuBar();
    void controlsUpdate();
    void DrawEdge(ImDrawList*, const std::string, const std::string);
    //bool MergeGroup(std::string, std :: string);
    //std :: string Find(std::string);
    //void addNode(std::string);

public:

    DSU(std::string, int&, float&, bool&);
    ~DSU();

    // public methods:
    void update();

};

