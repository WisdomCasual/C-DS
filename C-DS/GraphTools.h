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

    #define FIXED_NODE_COLOR ImGui::GetColorU32(IM_COL32(70, 70, 70, 255))

    #define DEFAULT_VERT_COL ImGui::GetColorU32(IM_COL32(150, 150, 150, 255))
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
        std::string u, v;
        long long w;
        bool weighted;
        ImU32 color;
        Edge(std::string u, std::string v) {
            this->u = u;
            this->v = v;
            w = 0;
            color = DEFAULT_EDGE_COL;
            weighted = false;
        }
        Edge(std::string u, std::string v, long long w)
            : Edge(u, v)
        {
            this->w = w;
            weighted = true;
        }
        bool operator < (const Edge& other) const{
            if(w != other.w)
                return w < other.w;
            else if (u != other.u)
                return u < other.u;
            else
                return v < other.v;
        }
    };

    std::map<std::string, Vertex> nodes;
    std::map<Edge, ImVec2> w_pos;

    std::set<Edge> edges;

    char graphText[1000];

    // private fields:
    ImGuiWindowFlags main_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGuiWindowFlags controls_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuiIO* io;

    int cur_tool = 0, activeAlgo = 0, directed = 0;
    float speed = 1.f, curTime = 0;
    bool found = false, cleared = true, paused = false;
    std::string dragging;
    ImVec2 camPos = { 0, 0 }, camTarget = { 0, 0 };

    // private methods:
    void updateMenuBar();
    void controlsUpdate();
    void clearStates();
    ImU32 ContrastingColor(ImU32);
    void DrawEdge(ImDrawList*, const Edge&);
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

