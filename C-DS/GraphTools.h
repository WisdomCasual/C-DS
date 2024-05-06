#pragma once
#include "GrandWindow.h"
#include <imgui.h>
#include <string>
#include <set>
#include <map>
#include <random>

class GraphTools :
    public GrandWindow
{
private:
    // graph settings:
    #define EDGE_LENGTH 200.f
    #define VERTEX_RADIUS 30.f * zoomScale

    #define FIXED_NODE_COLOR ImGui::GetColorU32(IM_COL32(30, 30, 70, 255))

    #define DEFAULT_VERT_COL ImGui::GetColorU32(IM_COL32(150, 150, 150, 255))
    #define ADJ_ROOT_COL ImGui::GetColorU32(IM_COL32(60, 60, 150, 255))
    #define ADJ_CHILD_COL ImGui::GetColorU32(IM_COL32(80, 80, 160, 255))
    #define ADJ_EDGE_COL ImGui::GetColorU32(IM_COL32(120, 120, 200, 255))

    #define START_POINT_COL ImGui::GetColorU32(IM_COL32(50, 50, 150, 255))
    #define END_POINT_COL ImGui::GetColorU32(IM_COL32(150, 50, 50, 255))

    #define DEFAULT_EDGE_COL ImGui::GetColorU32(IM_COL32(200, 200, 200, 255))

    // speed constraints:
    #define MAX_SPEED 100.0f
    #define MIN_SPEED 0.1f
    #define DELAY_TIME 0.2f

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

    struct Edge {
        int w = 0;
        bool weighted = false;
        ImVec2 pos = {0, 0};
        ImU32 color = DEFAULT_EDGE_COL;
    };

    std::map<std::string, Vertex> nodes;
    std::map<std::pair<std::string, std::string>, Edge> edges;
    std::map<std::string, std::set<std::pair<std::string, std::pair<int, bool>>>> adj;

    char graphText[1000];

    // private fields:
    ImGuiWindowFlags main_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGuiWindowFlags controls_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuiIO* io;

    int cur_tool = 0, activeAlgo = 0, directed = 0;
    float speed = 1.f, curTime = 0;
    bool found = false, cleared = true, paused = false, leftClickPressed = false, camFollow = false;
    std::string dragging;
    std::string viewAdjacent, startNode = "", endNode = "";
    ImVec2 camPos = { 0, 0 }, camTarget = { 0, 0 };

    // private methods:
    void updateMenuBar();
    void controlsUpdate();
    void clearStates();
    ImU32 ContrastingColor(ImU32);
    void pointToNode(const std::string, ImU32);
    void DrawEdge(ImDrawList*, const std::string, const std::string, Edge&);
    float calcDist(float, float, float, float);
    void graphUpdate();
    void dfs();
    void bfs();
    void dijkstra();

public:

    GraphTools(std::string, int&, float&, bool&);
    ~GraphTools();

    // public methods:
    void update();

};
