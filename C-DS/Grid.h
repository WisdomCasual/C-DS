#pragma once
#include "GrandWindow.h"
#include <imgui.h>
#include <vector>
#include <queue>
#include <stack>

class Grid :
    public GrandWindow
{
private:
    // grid size constraints:
    const int X_MAX = 50;
    const int X_MIN = 2;
    const int Y_MAX = 50;
    const int Y_MIN = 2;
    const float GRID_ROUNDNESS = 7.f;

    // speed constraints:
    const float GRID_MAX_SPEED = 100.0f;
    const float GRID_MIN_SPEED = 0.1f;
    const float GRID_DELAY_TIME = 0.2f;

    const float CELL_SIZE = 70.f;
    const float SEPARATOR_SIZE = 5.f;

    enum {
        PATH_CELL,
        VISITED_CELL,
        START_POS_CELL,
        END_POS_CELL,
        OBSTACLE_CELL,
        EMPTY_CELL,
        INQUE_CELL,

        OUTLINE_COL,
    };

    // private fields:
    ImGuiWindowFlags main_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGuiWindowFlags controls_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuiIO* io;

    int cur_tool = 0, x_size = 5, y_size = 5, activeAlgo = 0, obstaclesCnt = 0;
    float speed = 1.f, curTime = 0;
    bool diagonal_movement = false, found = false, cleared = false, paused = false, camFollow = false, movingCam = false;
    ImVec2 camPos = { 0, 0 }, camTarget = { 0, 0 };
    int prev_x = -1, prev_y = -1;

    std::pair<int, int> start_pos = { 0, 0 }, end_pos = { x_size - 1, y_size - 1 };
    std::pair<int, int> cur_node;

    const int dx[4] = { 1, 0, -1, 0 };
    const int dy[4] = { 0, 1, 0, -1 };
    const int dir[3] = { 0, 1, -1 };
    const double diagonal_cost = sqrt(2);

    struct Node {
        int x = -1, y = -1;
        Node() {};
        Node(int x, int y){
            this->x = x;
            this->y = y;
        }
        Node(std::pair<int, int> node) {
            x = node.first;
            y = node.second;
        }
    };
    struct djskNode {
        Node coordinates = {};
        double cost = 0;
        djskNode() {};
        djskNode(Node coordinates, double cost) {
            this->coordinates = coordinates;
            this->cost = cost;
        }
        friend bool operator<(const djskNode& l, const djskNode &r){
            return l.cost > r.cost;
        }
    };
    struct aStarNode {
        djskNode curNode = {};

        aStarNode() {};
        aStarNode(Node coordinates, double cost, double gVal){
            this->curNode = djskNode(coordinates, cost+gVal);
        }
        friend bool operator<(const aStarNode& l, const aStarNode& r) {
            return l.curNode < r.curNode;
        }
    };

    std::stack<std::pair<int, int>> dfs_stack;
    std::queue<std::pair<int, int>> bfs_queue;
    std::priority_queue<std::pair<double, std::pair<int, int>>> dijkstra_queue;
    std::priority_queue<aStarNode> astar_queue;


    std::vector<std::vector<int>> vis = std::vector<std::vector<int>>(X_MAX, std::vector<int>(Y_MAX));
    std::vector<std::vector<double>> cost = std::vector<std::vector<double>>(X_MAX, std::vector<double>(Y_MAX, FLT_MAX));
    std::vector<std::vector<bool>> is_obstacle = std::vector<std::vector<bool>>(X_MAX, std::vector<bool>(Y_MAX));
    std::vector<std::vector<std::pair<int, int>>> par = std::vector<std::vector<std::pair<int, int>>>(X_MAX, std::vector<std::pair<int, int>>(Y_MAX));

    // private methods:
    void controlsUpdate();
    void gridUpdate();
    void useTool(int, int);
    ImU32 getCellColor(int, int);
    ImU32 getColor(int);
    void clearVisited();
    bool inbounds(int, int);
    double getCost(int x, int y);
    void followCell(int, int);
    void dfs();
    void bfs();
    void dijkstra();
    double g_val(Node a);
    void a_star();
    void st_line_eq(ImVec2, float);

public:

    Grid(std::string, int&, float&, bool&, int&);
    ~Grid();

    // public methods:
    void update();


};

