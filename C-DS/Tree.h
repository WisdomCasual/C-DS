#pragma once
#include "GrandWindow.h"
#include <imgui.h>
#include <map>
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

	struct CustomStringSort {
		bool operator()(const std::string& first, const std::string& second) const {
			if (first.size() != second.size()) return first.size() < second.size();
			return first < second;
		}
	};

	// private fields:

	ImGuiWindowFlags main_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
	ImGuiWindowFlags controls_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGuiIO* io;

	ImVec2 camPos = { 0, 0 }, camTarget = { 0, 0 };
	float speed = 1.f, curTime = 0;
	bool movingCam = false, leftClickPressed = false;
	std::string node_u, node_v, cur_node_u, cur_node_v, viewComponent;

	std::map<std::string, std::string> parent;
	std::map<std::string, Vertex, CustomStringSort> nodes;

	std::string dragging;

    int n,cur_tool = 0;
    std::map<std::string, int> node_level;
    char add_node_text[20] = {};
	std::string root = "\0";

	// private methods:
	ImU32 ContrastingColor(ImU32);
	void graphUpdate();
	float calcDist(float, float, float, float);
	void updateMenuBar();
	void controlsUpdate();
	void drawEdge(ImDrawList*, const std::string, const std::string);
	void addNode(std::string, std::string);

public:

    Tree(std::string, int&, float&, bool&);
    ~Tree();

	// public methods:
	void update();

};

