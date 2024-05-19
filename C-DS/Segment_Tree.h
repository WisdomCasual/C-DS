#pragma once
#include "GrandWindow.h"
#include <imgui.h>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <numeric>
#include <cmath>
class Segment_Tree :

	public GrandWindow
{
private:
#define VERTEX_RADIUS 30.f * zoomScale
#define UPDATED_NODE_COLOR ImGui::GetColorU32(IM_COL32(50, 150, 50, 255))
#define FIXED_NODE_COLOR ImGui::GetColorU32(IM_COL32(40, 40, 40, 255))
#define VIS_VERT_COL ImGui::GetColorU32(IM_COL32(50, 150, 50, 255))
#define INCORRECT ImGui::GetColorU32(IM_COL32(150, 50, 50, 255))

#define DEFAULT_VERT_COL ImGui::GetColorU32(IM_COL32(150, 150, 150, 255))
#define COMP_VERT_COL ImGui::GetColorU32(IM_COL32(50, 150, 150, 255))

	// speed constraints:
#define SEGTREE_MAX_SPEED 10.0f
#define SEGTREE_MIN_SPEED 0.1f
#define SEGTREE_DELAY 1.f
	struct Vertex {
		int minimum = INT_MAX, maximum = INT_MIN, sum = 0, gcd = 0, xr = 0;
		float x, y;
		float fx = 0, fy = 0;
		bool fixed = true;
		ImU32 color;
		Vertex() {
			x = (rand() % 10000) / 100.f;
			y = (rand() % 10000) / 100.f;
			color = DEFAULT_VERT_COL;
		}
	};

	ImVec2 camPos = { 0, 0 }, camTarget = { 0, 0 };
	float speed = 1.f, curTime = 0;
	bool movingCam = false, leftClickPressed = false, camFollow = false;

	std::string word;

	int cur_tool = 0;
	char add_node_text[4][20] = {};

	int sz = 0, level = 0;


	Vertex Neutral;
	std::vector<Vertex>Tree;

	ImU32 ContrastingColor(ImU32);
	void graphUpdate();
	float calcDist(float, float, float, float);
	void controlsUpdate();
	void drawEdge(ImDrawList*, int, int);



	Vertex Query(int, int, int, int, int);
	Vertex Query(int, int);
	Vertex Query(int);
	Vertex Merge(Vertex, Vertex);
	Vertex Single(int);


	void update(int, int, int, int, int);
	void update(int, int);

	// node , s , e 
	void build(int, int, int);

	void clear();

	Segment_Tree(std::string, int&, float&, bool&);
	~Segment_Tree();

	// public methods:
	void update();

};