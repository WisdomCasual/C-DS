#pragma once
#include "GrandWindow.h"
#include <imgui.h>
#include <vector>


class HashMap :
    public GrandWindow
{
    // grid size constraints:
    #define MAX_TABLE_SIZE 50
    #define MIN_TABLE_SIZE 2

    #define HM_SEPARATOR_SIZE 100.f * zoomScale
    #define CELL_SIZE_X 180.f * zoomScale
    #define CELL_SIZE_Y 60.f * zoomScale

    #define DEFAULT_BUCKET_COL ImGui::GetColorU32(IM_COL32(150, 150, 150, 255))
    #define CUR_BUCKET_COL ImGui::GetColorU32(IM_COL32(50, 150, 50, 255))
    #define FAIL_BUCKET_COL ImGui::GetColorU32(IM_COL32(150, 50, 50, 255))

    #define VERTEX_RADIUS 30.f * zoomScale
    #define NODES_DIST 200.f * zoomScale
    #define LINK_DIST 120.f
    #define DEFAULT_NODE_COL ImGui::GetColorU32(IM_COL32(150, 150, 150, 255))
    #define ITER_NODE_COL ImGui::GetColorU32(IM_COL32(50, 150, 150, 255))
    #define FAIL_NODE_COL ImGui::GetColorU32(IM_COL32(150, 50, 50, 255))
    #define FOUND_NODE_COL ImGui::GetColorU32(IM_COL32(50, 150, 50, 255))
    #define DEFAULT_EDGE_COL ImGui::GetColorU32(IM_COL32(200, 200, 200, 255))
    #define TEXT_COL ImGui::GetColorU32(IM_COL32(255, 255, 255, 255))

    // speed constraints:
    #define HM_MAX_SPEED 5.0f
    #define HM_MIN_SPEED 0.5f
    #define HM_DELAY 1.f

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
    void controlsUpdate();
    void tableUpdate();
    float calcDist(float, float, float, float);
    void drawEdge(ImVec2, ImVec2);
    void followNode(ImVec2);
    int hashValue(std::string);

public:

    HashMap(std::string, int&, float&, bool&);
    ~HashMap();

    // public methods:
    void update();


};

