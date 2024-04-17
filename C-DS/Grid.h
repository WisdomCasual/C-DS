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
    #define X_MAX 50
    #define X_MIN 2
    #define Y_MAX 50
    #define Y_MIN 2

    // speed constraints:
    #define MAX_SPEED 100.0f
    #define MIN_SPEED 0.1f
    #define DELAY_TIME 0.2f

    // private fields:
    ImGuiWindowFlags main_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGuiWindowFlags controls_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuiIO* io;

    int cur_tool = 0, x_size = 5, y_size = 5, activeAlgo = 0;
    float speed = 1.f, curTime = 0;
    bool diagonal_movement = false, found = false, cleared = true, paused = false;
    ImVec2 offset = {0, 0};

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
        double gVal = 0;
        djskNode curNode = {};

        aStarNode() {};
        aStarNode(Node coordinates, double cost, double gVal){
            this->curNode = djskNode(coordinates, cost+gVal);
            this->gVal = gVal;
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
    std::vector<std::vector<bool>> is_obstacle = std::vector<std::vector<bool>>(X_MAX, std::vector<bool>(Y_MAX));
    std::vector<std::vector<std::pair<int, int>>> par = std::vector<std::vector<std::pair<int, int>>>(X_MAX, std::vector<std::pair<int, int>>(Y_MAX));

    // private methods:
    void updateMenuBar();
    void controlsUpdate();
    void gridUpdate();
    void useTool(int, int);
    ImU32 getColor(int, int);
    void clearVisited();
    bool inbounds(int, int);
    double getCost(int x, int y);
    void dfs();
    void bfs();
    void dijkstra();
    double g_val(Node a);
    void a_star();

public:

    Grid(std::string, int&, float&, bool&);
    ~Grid();

    // public methods:
    void update();


};

