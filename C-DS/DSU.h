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
    const float EDGE_LENGTH = 200.f;
    const float VERTEX_RADIUS = 30.f;

    const int BY_SIZE = 1;
    const int BY_HEIGHT = 2;

    // speed constraints:
    const float MAX_SPEED = 10.0f;
    const float MIN_SPEED = 0.1f;
    const float DELAY = 1.f;
    
    const char NO_DRAGGING = char(2);

    enum {
        FIXED_NODE_COL,
        VIS_VERT_COL,
        DEFAULT_VERT_COL,
        COMP_VERT_COL,
        VERT_BORDER_COL,
        DEFAULT_EDGE_COL,
        TEXT_COL
    };
    
    struct Vertex {
        float x, y;
        float fx = 0, fy = 0;
        bool fixed = false, searching = false, beingDragged = false;
        int color;
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
    int cur_tool = 0, rank_type = 0;
    float speed = 1.f, curTime = 0;
    bool movingCam = false, leftClickPressed = false, pathCompression = false, isMerging = false;
    std::string node_u, node_v, cur_node_u, cur_node_v, viewComponent;

    std::map<std::string, int> st_height, st_size;
    std::map<std::string, std::string> parent;
    std::map<std::string, Vertex, CustomStringSort> nodes;
    char add_node_text[50] = {};

    std::string dragging;

    // private methods:

    void updateDraggedComponent();
    void graphUpdate();
    float calcDist(float, float, float, float);
    ImU32 getColor(int color_code);
    void controlsUpdate();
    void drawEdge(ImDrawList*, const std::string, const std::string);
    void mergeGroup(const std::string, const std::string);
    const std::string find(const std::string);
    void addNode(std::string);
    bool sameGroup(const std::string, const std::string);

public:

    DSU(std::string, int&, float&, bool&, int&);
    ~DSU();

    // public methods:
    void update();

};

