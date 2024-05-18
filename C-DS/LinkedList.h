#pragma once
#include "GrandWindow.h"
#include <imgui.h>
#include <set>


class LinkedList :
    public GrandWindow
{
    #define VERTEX_RADIUS 30.f * zoomScale
    #define EDGE_LENGTH 200.f * zoomScale
    #define DEFAULT_NODE_COL ImGui::GetColorU32(IM_COL32(150, 150, 150, 255))
    #define ITER_VERT_COL ImGui::GetColorU32(IM_COL32(50, 150, 150, 255))
    #define TEXT_COL ImGui::GetColorU32(IM_COL32(255, 255, 255, 255))
    #define DEFAULT_EDGE_COL ImGui::GetColorU32(IM_COL32(200, 200, 200, 255))

    // speed constraints:
    #define LL_MAX_SPEED 5.0f
    #define LL_MIN_SPEED 0.5f
    #define LL_DELAY 1.f

    // private fields:
    ImGuiWindowFlags main_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGuiWindowFlags controls_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const ImGuiIO* io = &ImGui::GetIO();

    int cur_tool = 0, highlighted_idx = -1, selected_index = 0, iterationMode = 0;
    std::string insert_val;
    float speed = 1.f, curTime = 0;
    bool paused = false, camFollow = false, movingCam = false;
    ImVec2 camPos = { 0, 0 }, camTarget = { 0, 0 };

    struct Node {
        std::string value;
        Node* next;
        ImVec2 curPos;
        Node(std::string value, ImVec2 curPos) {
            this->value = value;
            this->curPos = curPos;
            next = nullptr;
        }
    };

    int listSize = 0;
    Node *head, *tail, *curNode, *tempNode;

    char add_node_text[10] = {};

    // private methods:
    void pushFront();
    void pushBack();
    void popFront();
    Node* reverse(Node*);
    void controlsUpdate();
    float calcDist(float, float, float, float);
    void drawEdge(ImVec2&, ImVec2&);
    void listUpdate();
    void followNode(ImVec2);

public:

    LinkedList(std::string, int&, float&, bool&);
    ~LinkedList();

    // public methods:
    void update();


};

