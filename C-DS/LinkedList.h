#pragma once
#include "GrandWindow.h"
#include <imgui.h>
#include <set>
#include <queue>


class LinkedList :
    public GrandWindow
{
    const float LL_VERTEX_RADIUS = 30.f;
    const float NODES_DIST = 150.f;

    // speed constraints:
    const float LL_MAX_SPEED = 5.0f;
    const float LL_MIN_SPEED = 0.5f;
    const float LL_DELAY = 1.f;

    enum {
        DEF_VERT_COL,
        ITER_VERT_COL,
        VERT_BORDER_COL,
        DEFAULT_EDGE_COL,
        TEXT_COL,
        TEXT_OUTLINE_COL,

        PUSH_BACK,
        PUSH_FRONT,
        INSERT,
        IDLE
    };

    // private fields:
    ImGuiWindowFlags main_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGuiWindowFlags controls_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

    
    

    int cur_tool = 0, highlighted_idx = -1, selected_index = 0, iterationMode = 0;;
    std::string insert_val;
    float speed = 1.f, curTime = 0;
    bool paused = false, camFollow = false, movingCam = false;
    
    std::queue<std::string> pending;

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

    int listSize = 0, mode = IDLE;
    Node* head = nullptr, * tail = nullptr, * curNode = nullptr, * tempNode = nullptr;

    char add_element_content[200] = {};

    // private methods:
    ImU32 getColor(int color_code);
    void drawText(ImVec2, const char*);
    void pushFront(std::string);
    void pushBack(std::string);
    void popFront();
    void getInput();
    Node* reverse(Node*);
    void controlsUpdate();
    float calcDist(float, float, float, float);
    void drawEdge(ImVec2&, ImVec2&);
    void listUpdate();
    void followNode(ImVec2);

public:

    LinkedList(std::string, int&, float&, bool&, int&);
    ~LinkedList();

    // public methods:
    void update();


};

