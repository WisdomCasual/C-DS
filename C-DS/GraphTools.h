#pragma once
#include "GrandWindow.h"
#include <imgui.h>
#include <string>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <vector>

class GraphTools :
    public GrandWindow
{
private:
    // graph settings:
    #define EDGE_LENGTH 200.f
    #define VERTEX_RADIUS 30.f * zoomScale

    #define FIXED_NODE_COLOR ImGui::GetColorU32(IM_COL32(40, 40, 40, 255))

    #define DEFAULT_VERT_COL ImGui::GetColorU32(IM_COL32(150, 150, 150, 255))
    #define ADJ_ROOT_COL ImGui::GetColorU32(IM_COL32(60, 60, 150, 255))
    #define ADJ_CHILD_COL ImGui::GetColorU32(IM_COL32(80, 80, 160, 255))
    #define VIS_VERT_COL ImGui::GetColorU32(IM_COL32(50, 150, 50, 255))
    #define PATH_VERT_COL ImGui::GetColorU32(IM_COL32(50, 150, 150, 255))
    #define INQUE_VERT_COL ImGui::GetColorU32(IM_COL32(70, 70, 70, 255))
    #define ADJ_EDGE_COL ImGui::GetColorU32(IM_COL32(120, 120, 200, 255))

    #define START_POINT_COL ImGui::GetColorU32(IM_COL32(50, 50, 150, 255))
    #define END_POINT_COL ImGui::GetColorU32(IM_COL32(150, 50, 50, 255))

    #define DEFAULT_EDGE_COL ImGui::GetColorU32(IM_COL32(200, 200, 200, 255))
    #define VIS_EDGE_COL ImGui::GetColorU32(IM_COL32(80, 180, 80, 255))
    #define PATH_EDGE_COL ImGui::GetColorU32(IM_COL32(80, 180, 180, 255))
    #define INQUE_EDGE_COL ImGui::GetColorU32(IM_COL32(120, 180, 120, 255))
    #define CANCELED_EDGE_COL ImGui::GetColorU32(IM_COL32(50, 50, 50, 255))


    // speed constraints:
    #define GRAPH_MAX_SPEED 20.0f
    #define GRAPH_MIN_SPEED 0.1f
    #define GRAPH_DELAY 1.f

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
        bool directed = true;
        ImVec2 pos = {0, 0};
        ImU32 color = DEFAULT_EDGE_COL;
    };

    // private fields:

    std::map<std::string, Vertex> nodes;
    std::map<std::pair<std::string, std::string>, Edge> edges;
    std::map<std::string, std::vector<std::pair<std::string, std::pair<int, bool>>>> adj;

    char graphText[1000];

    ImGuiWindowFlags main_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGuiWindowFlags controls_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuiIO* io;

    int cur_tool = 0, activeAlgo = 0, directed = 0, found = 0;
    float speed = 1.f, curTime = 0;
    bool cleared = true, paused = false, leftClickPressed = false, camFollow = false, movingCam = false;
    std::string dragging, cur_node;
    std::string viewAdjacent, startNode = "", endNode = "";
    ImVec2 camPos = { 0, 0 }, camTarget = { 0, 0 };

    std::stack<std::pair< std::string, int>> dfs_stack;
    std::queue<std::string> bfs_queue;
    std::priority_queue<std::pair<long long, std::string>> dijkstra_queue;
    std::map<std::string, long long> vis;
    std::map<std::pair<std::string, std::string>, long long> edge_vis;
    std::map<std::string, std::string> par;
    std::map<std::string, std::vector<std::string>>path;

    // private methods:
    void controlsUpdate();
    void generateGraph();
    void clearStates();
    ImU32 ContrastingColor(ImU32);
    void pointToNode(const std::string, ImU32);
    void drawEdge(ImDrawList*, const std::string, const std::string, Edge&);
    float calcDist(float, float, float, float);
    void graphUpdate();
    void followNode(const std::string);
    void dfs();
    void bfs();
    void dijkstra();
    void bellmanFord();

public:

    GraphTools(std::string, int&, float&, bool&);
    ~GraphTools();

    // public methods:
    void update();

};

