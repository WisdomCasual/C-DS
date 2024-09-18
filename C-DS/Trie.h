#pragma once
#include "GrandWindow.h"
#include <imgui.h>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <queue>
#include <stack>
// trie done
class Trie :
	public GrandWindow
{
private:
	// graph settings:
	const float TRIE_EDGE_LENGTH = 100.f;
	const float TRIE_VERTEX_RADIUS = 30.f;

	// speed constraints:
	const float TRIE_MAX_SPEED = 10.0f;
	const float TRIE_MIN_SPEED = 0.1f;
	const float TRIE_DELAY = 1.f;

	enum {
		UPDATED_NODE_COLOR,
		FIXED_VERT_COLOR,
		VISITED_VERT_COL,
		INCORRECT,
		DEF_VERT_COL,
		VERT_BORDER_COL,
		MARKED_VERT_COL,
		DEFAULT_EDGE_COL,

		TEXT_COL,
		TEXT_OUTLINE_COL
	};

	struct Vertex {
		std::map<char, int>children;
		int cnt = 0;
		std::string val = "\0";
		int endofword = 0;
		float x, y;
		float fx = 0, fy = 0;
		bool fixed = false, searching = false;
		int color;
		Vertex() {
			x = (rand() % 10000) / 100.f;
			y = (rand() % 10000) / 100.f;
			color = DEF_VERT_COL;
		}
	};

	// private fields:

	ImGuiWindowFlags main_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
	ImGuiWindowFlags controls_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

	
	ImGuiIO* io = &ImGui::GetIO();

	
	float speed = 1.f, curTime = 0;
	bool movingCam = false, leftClickPressed = false;
	bool disableButtons = false, trueUpdate = false, camFollow = false;
	bool deleting = false;
	int curNode = -1, curIdx = -1 , endw = 0;

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
	ImU32 getColor(int);
	void drawText(ImVec2, const char*);
	void graphUpdate();
	float calcDist(float, float, float, float);
	void controlsUpdate();
	void drawEdge(ImDrawList*, int, int);
	void insertWord();
	bool search(std::string word, bool);
	void clear();

public:

	Trie(std::string, int&, float&, bool&, int&);
	~Trie();

	// public methods:
	void update();

};

