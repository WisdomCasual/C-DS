#pragma once
#include "GrandWindow.h"
#include <imgui.h>
#include <vector>


class HashMap :
    public GrandWindow
{
    // grid size constraints:
    const int MAX_TABLE_SIZE = 50;
    const int MIN_TABLE_SIZE = 2;

    const float HM_SEPARATOR_SIZE = 100.f;
    const float CELL_SIZE_X = 180.f;
    const float CELL_SIZE_Y = 60.f;
    
    const float BUCKET_ROUNDNESS = 10.f;
    const float HM_VERTEX_RADIUS = 30.f;
    const float NODES_DIST = 200.f;
    const float LINK_DIST = 120.f;;

    // speed constraints:
    const float HM_MAX_SPEED = 5.0f;
    const float HM_MIN_SPEED = 0.5f;
    const float HM_DELAY = 1.f;

    enum {
        DEFAULT_BUCKET_COL,
        BUCKET_BORDER_COL,
        CUR_BUCKET_COL,
        FAIL_BUCKET_COL,

        DEFAULT_NODE_COL,
        NODE_BORDER_COL,
        ITER_NODE_COL,
        FAIL_NODE_COL,
        FOUND_NODE_COL,
        DEFAULT_EDGE_COL,
        TEXT_COL,
        TEXT_OUTLINE_COL
    };


private:
    // private fields:
    ImGuiWindowFlags main_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGuiWindowFlags controls_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const ImGuiIO* io = &ImGui::GetIO();

    int cur_tool = 0, table_size_slider = 10, table_size = 10, cur_bucket = -1, mode = 0;
    float speed = 1.f, curTime = 0;
    bool paused = false, camFollow = false, movingCam = false, found = false;;
    ImVec2 camPos = { 0, 0 }, camTarget = { 0, 0 };
    ImVec2 center = { 0, 0 };
    std::string searchKey, newVal, toDelete;

    struct Node {
        std::string key, value;
        Node* next;
        ImVec2 curPos;
        ImU32 keyColor;
        ImU32 valColor;
        Node(std::string key, std::string value, ImVec2 curPos) {
            this->key = key;
            this->value = value;
            this->curPos = curPos;
            next = nullptr;
            keyColor = valColor = DEFAULT_NODE_COL;
        }
    };

    std::vector<Node*> buckets;
    Node* iteratingNode = nullptr, * tempNode = nullptr;

    char key_text[10] = "", value_text[10] = "";

    // private methods:
    ImU32 getColor(int color_code);
    void drawText(ImVec2, const char*);
    void controlsUpdate();
    void tableUpdate();
    float calcDist(float, float, float, float);
    void drawEdge(ImVec2, ImVec2);
    void followNode(ImVec2);
    int hashValue(std::string);

public:

    HashMap(std::string, int&, float&, bool&, int&);
    ~HashMap();

    // public methods:
    void update();


};

