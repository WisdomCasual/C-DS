#pragma once
#include "GrandWindow.h"
#include <imgui.h>
#include <map>
#include <unordered_map>
#include <set>
#include <string>
class Tree :
	public GrandWindow
{
private:
	// graph settings:
#define EDGE_LENGTH 200.f

#define VERTEX_RADIUS 30.f * zoomScale
#define ROOT_NODE_COLOR ImGui::GetColorU32(IM_COL32(50, 150, 50, 255))
#define FIXED_NODE_COLOR ImGui::GetColorU32(IM_COL32(40, 40, 40, 255))
#define VIS_VERT_COL ImGui::GetColorU32(IM_COL32(50, 150, 50, 255))

#define DEFAULT_VERT_COL ImGui::GetColorU32(IM_COL32(150, 150, 150, 255))
#define COMP_VERT_COL ImGui::GetColorU32(IM_COL32(50, 150, 150, 255))

// speed constraints:
#define DSU_MAX_SPEED 10.0f
#define DSU_MIN_SPEED 0.1f
#define DSU_DELAY 1.f

	struct Vertex {
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

	
	

	
	float speed = 1.f, curTime = 0;
	bool movingCam = false, leftClickPressed = false, disabled=false;
	std::string node_u, node_v, cur_node_u, cur_node_v, viewComponent;

	std::unordered_map<std::string, std::string> parent;
	std::unordered_map<std::string, Vertex> nodes;

	std::string dragging;

    int n,cur_tool = 0;
    std::unordered_map<std::string, int> node_level;
    char add_node_text[20] = {};
	std::string root = "\0";

	// private methods:
	ImU32 ContrastingColor(ImU32);
	void graphUpdate();
	float calcDist(float, float, float, float);
	void controlsUpdate();
	void drawEdge(ImDrawList*, const std::string, const std::string);
	void addNode(std::string, std::string);

public:

    Tree(std::string, int&, float&, bool&, int&);
    ~Tree();

	// public methods:
	void update();

};

