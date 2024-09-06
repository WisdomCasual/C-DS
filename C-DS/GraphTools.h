#pragma once
#include "GrandWindow.h"
#include <imgui.h>
#include <string>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <vector>

class GraphTools :
    public GrandWindow
{
private:
    // graph settings:
    const float EDGE_LENGTH = 200.f;
    const float VERTEX_RADIUS = 30.f;

    const char NO_DRAGGING = char(2);

    enum {

        FIXED_NODE_COLOR,

        DEFAULT_VERT_COL,
        ADJ_ROOT_COL,
        ADJ_CHILD_COL,
        VIS_VERT_COL,
        PATH_VERT_COL,
        INQUE_VERT_COL,
        ADJ_EDGE_COL,
        VERT_BORDER_COL,

        START_POINT_COL,
        END_POINT_COL,

        DEFAULT_EDGE_COL,
        VIS_EDGE_COL,
        PATH_EDGE_COL,
        INQUE_EDGE_COL,
        CANCELED_EDGE_COL,

        TEXT_COL
    };


    // speed constraints:
    #define GRAPH_MAX_SPEED 20.0f
    #define GRAPH_MIN_SPEED 0.1f
    #define GRAPH_DELAY 1.f

    struct Vertex {
        float x, y;
        float fx = 0, fy = 0;
        bool fixed = false, beingDragged = false;
        int color;
        Vertex() {
            x = (rand() % 10000) / 100.f;
            y = (rand() % 10000) / 100.f;
            color = DEFAULT_VERT_COL;
        }
    };

    struct Edge {
        int w = 0;
        bool weighted = false;
        bool directed = true;
        ImVec2 pos = {0, 0};
        int color = DEFAULT_EDGE_COL;
    };

	class DSU {
	private:
        std::map<std::string, std::string> parent;
		std::map<std::string, int> st_size;
	public:
		DSU(std::map<std::string, Vertex>& nodes) {

            for (auto node : nodes){
                parent[node.first] = node.first;
                st_size[node.first] = 1;
            }

		}
		std::string find(const std::string& x) {
			return (parent[x] == x ? x : parent[x] = find(parent[x]));
		}
        bool isConnected(const std::string& u, const std::string& v) {
            return (find(u) == find(v));
        }
		void Union(const std::string& u, const std::string& v) {
			std::string set_u = find(u);
			std::string set_v = find(v);
			if (set_u == set_v)
				return;
			if (st_size[set_u] < st_size[set_v])
				parent[set_u] = set_v, st_size[set_v] += st_size[set_u];
			else
				parent[set_v] = set_u, st_size[set_u] += st_size[set_v];
		}
	};

    // private fields:

    std::map<std::string, Vertex> nodes;
    std::map<std::pair<std::string, std::string>, Edge> edges;
    std::map<std::string, std::vector<std::pair<std::string, std::pair<int, bool>>>> adj;
    DSU* mst_dsu = nullptr;

    char graphText[500000];

    ImGuiWindowFlags main_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGuiWindowFlags controls_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuiIO* io;

    int cur_tool = 0, activeAlgo = 0, directed = 0, found = 0;
    float speed = 1.f, curTime = 0;
    bool cleared = true, paused = false, leftClickPressed = false, camFollow = false, movingCam = false, weighted_rand = false;
    bool showNodeText = true, showWeightText = true;
    std::string dragging, cur_node;
    std::string viewAdjacent, startNode = "", endNode = "";
    ImVec2 camPos = { 0, 0 }, camTarget = { 0, 0 };

    std::stack<std::pair< std::string, int>> dfs_stack;
    std::queue<std::string> bfs_queue;
    std::priority_queue<std::pair<long long, std::string>> dijkstra_queue;
	std::priority_queue<std::pair<long long, std::pair<std::string, std::string>>> kruskal_queue;
    std::map<std::string, long long> vis;
    std::map<std::pair<std::string, std::string>, long long> edge_vis;
    std::map<std::string, std::string> par;
    std::map<std::string, std::vector<std::string>>path;

    // private methods:
    void controlsUpdate();
    void generateGraph();
    void clearStates();
    void pointToNode(const std::string, ImU32);
    void drawEdge(ImDrawList*, const std::string, const std::string, Edge&);
    float calcDist(float, float, float, float);
    void updateDraggedComponent();
    void graphUpdate();
    void followNode(const std::string);
    void dfs();
    void bfs();
    void dijkstra();
    void kruskal();
    void bellmanFord();
    ImU32 getColor(int);

public:

    GraphTools(std::string, int&, float&, bool&, int&);
    ~GraphTools();

    // public methods:
    void update();

};

