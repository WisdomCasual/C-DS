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
    #define VIS_VERT_COL ImGui::GetColorU32(IM_COL32(50, 150, 50, 255))

    #define DEFAULT_VERT_COL ImGui::GetColorU32(IM_COL32(150, 150, 150, 255))
    #define COMP_VERT_COL ImGui::GetColorU32(IM_COL32(50, 150, 150, 255))

    #define BY_SIZE 1
    #define BY_HEIGHT 2

    // speed constraints:
    #define MAX_SPEED 10.0f
    #define MIN_SPEED 0.1f
    #define DELAY 1.f
    
    struct Vertex {
        float x, y;
        float fx = 0, fy = 0;
        bool fixed = false, searching = false;
        ImU32 color;
        Vertex() {
            x = (rand() % 10000) / 100.f;
            y = (rand() % 10000) / 100.f;
            color = DEFAULT_VERT_COL;
        }
    };

    struct CustomStringSort {
		bool operator()(const std::string& first, const std::string& second) const {
            if (first.size() != second.size()) return first.size() < second.size();
			return first < second;
		}
    };

    // private fields:

    ImGuiWindowFlags main_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGuiWindowFlags controls_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuiIO* io;

    ImVec2 camPos = { 0, 0 }, camTarget = { 0, 0 };
    int cur_tool = 0, rank_type;
    float speed = 1.f, curTime = 0;
    bool movingCam = false, leftClickPressed = false, pathCompression = false, isMerging = false;
    std::string node_u, node_v, cur_node_u, cur_node_v, viewComponent;

    std::map<std::string, int> st_height, st_size;
    std::map<std::string, std::string> parent;
    std::map<std::string, Vertex, CustomStringSort> nodes;
    char add_node_text[50];

    std::string dragging;

    // private methods:

    ImU32 ContrastingColor(ImU32);
    void graphUpdate();
    float calcDist(float, float, float, float);
    void updateMenuBar();
    void controlsUpdate();
    void drawEdge(ImDrawList*, const std::string, const std::string);
    void mergeGroup(const std::string, const std::string);
    const std::string find(const std::string);
    void addNode(std::string);
    bool sameGroup(const std::string, const std::string);

public:

    DSU(std::string, int&, float&, bool&);
    ~DSU();

    // public methods:
    void update();

};

