#pragma once
#include "GrandWindow.h"
#include <imgui.h>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <queue>
#include <stack>
class Trie :
	public GrandWindow
{
private:
	// graph settings:
#define TRIE_EDGE_LENGTH 100.f

#define VERTEX_RADIUS 30.f * zoomScale
#define UPDATED_NODE_COLOR ImGui::GetColorU32(IM_COL32(50, 150, 50, 255))
#define FIXED_NODE_COLOR ImGui::GetColorU32(IM_COL32(40, 40, 40, 255))
#define VIS_VERT_COL ImGui::GetColorU32(IM_COL32(50, 150, 50, 255))
#define INCORRECT ImGui::GetColorU32(IM_COL32(150, 50, 50, 255))

#define DEFAULT_VERT_COL ImGui::GetColorU32(IM_COL32(150, 150, 150, 255))
#define COMP_VERT_COL ImGui::GetColorU32(IM_COL32(50, 150, 150, 255))

// speed constraints:
#define TRIE_MAX_SPEED 10.0f
#define TRIE_MIN_SPEED 0.1f
#define TRIE_DELAY 1.f

	struct Vertex {
		std::map<char, int>children;
		int cnt = 0;
		std::string val = "\0";
		bool endofword = false;
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

	// private fields:

	ImGuiWindowFlags main_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
	ImGuiWindowFlags controls_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGuiIO* io = &ImGui::GetIO();

	ImVec2 camPos = { 0, 0 }, camTarget = { 0, 0 };
	float speed = 1.f, curTime = 0;
	bool movingCam = false, leftClickPressed = false;
	bool disableButtons = false, trueUpdate = false, camFollow = false;
	bool deleting = false;
	int curNode = -1, curIdx = -1;

	//-_-
	int dragging = -1;
	std::vector<int>parent;
	std::vector<Vertex>nodes;
	std::queue<int>updatedNodes;
	std::stack<int>deletedNodes;

	std::string word;

	int cur_tool = 0;
	char add_node_text[4][20] = {};

	// private methods:
	ImU32 ContrastingColor(ImU32);
	void graphUpdate();
	float calcDist(float, float, float, float);
	void controlsUpdate();
	void drawEdge(ImDrawList*, int, int);
	void insertWord();
	bool search(std::string word, bool);
	void clear();

public:

	Trie(std::string, int&, float&, bool&);
	~Trie();

	// public methods:
	void update();

};

