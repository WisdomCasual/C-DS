#pragma once
#include "GrandWindow.h"
#include <imgui.h>
#include <vector>
#include <queue>


class HashTable :
    public GrandWindow
{
    // grid size constraints:
    const int MAX_TABLE_SIZE = 50;
    const int MIN_TABLE_SIZE = 2;

    const float HT_SEPARATOR_SIZE = 70.f;
    const float CELL_SIZE_X = 180.f;
    const float CELL_SIZE_Y = 60.f;

    const float BUCKET_ROUNDNESS = 10.f;
    const float HT_VERTEX_RADIUS = 30.f;
    const float HT_NODES_DIST = 200.f;

    // speed constraints:
    const float HT_MAX_SPEED = 5.0f;
    const float HT_MIN_SPEED = 0.5f;
    const float HT_DELAY = 1.f;

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
        TEXT_OUTLINE_COL,

        INSERT,
        FIND,
        ERASE,
        IDLE
    };

private:
    // private fields:
    ImGuiWindowFlags main_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGuiWindowFlags controls_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const ImGuiIO* io = &ImGui::GetIO();

    int cur_tool = 0, table_size_slider = 10,  table_size = 10, cur_bucket = -1, mode = IDLE;
    float speed = 1.f, curTime = 0;
    bool paused = false, camFollow = false, movingCam = false, found = false;;
    ImVec2 camPos = { 0, 0 }, camTarget = { 0, 0 };
    ImVec2 center = { 0, 0 };
    std::string searchVal, toDelete;

    struct Node {
        std::string value;
        Node* next;
        ImVec2 curPos;
        ImU32 color;
        Node(std::string value, ImVec2 curPos) {
            this->value = value;
            this->curPos = curPos;
            next = nullptr;
            color = DEFAULT_NODE_COL;
        }
    };

    std::vector<Node*> buckets;
    Node* iteratingNode = nullptr, *tempNode = nullptr;
    std::queue<std::string> pending;

    char add_element_content[200] = "";

    // private methods:
    ImU32 getColor(int color_code);
    void drawText(ImVec2, const char*);
    void getInput();
    void selectBucket(int);
    void controlsUpdate();
    void tableUpdate();
    float calcDist(float, float, float, float);
    void drawEdge(ImVec2, ImVec2);
    void followNode(ImVec2);
    int hashValue(std::string);

public:

    HashTable(std::string, int&, float&, bool&, int&);
    ~HashTable();

    // public methods:
    void update();


};

