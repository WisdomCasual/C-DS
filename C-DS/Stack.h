#pragma once
#include "GrandWindow.h"
#include <imgui.h>
#include <queue>
#include <string>
class Stack : public GrandWindow
{
private:

	const int STACK_MAX_SIZE = 1024;

	// speed constraints:
	const float STACK_MAX_SPEED = 10.0f;
	const float STACK_MIN_SPEED = 0.1f;
	const float STACK_DELAY_TIME = 0.2f;
	const float STACK_BASE_DELAY = 0.6f;

	const float STACK_ROUNDNESS = 10.f;
	const float STACK_CELL_SIZE = 70.f;
	const float STACK_SEPARATOR_SIZE = 5.f;

	enum {
		DEFAULT_CELL_COL,
		CELL_BORDER_COL,
		END_CELL_COL,
		ARROW1_COL,
		ARROW2_COL,
		TEXT_COLOR
	};

// private fields:
	ImGuiWindowFlags main_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
	ImGuiWindowFlags controls_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGuiIO* io;

	// tools for stack:
	// 1: enqueue
	// 2: dequeue
	// 3: expand (behind the scenes) (for later visualize the the expansion process : ~visualize the 2 contentays~) 
	int cur_tool = 0, sz = 0, headpointer = 0, currentMaxSize = 1, expansion = -1, tempSize = 0;
	bool camfollow = false, movingCam = false;
	ImVec2 camPos = { 0, 0 }, camTarget = { 0, 0 };
	char add_element_content[200] = {};
	float speed = 1.f, passedTime = 0.f;

	std::string* content = new std::string[currentMaxSize], * tempContent = nullptr;
	std::queue<std::string> pending;

	
	void controlsUpdate();

	ImU32 getColor(int);

	//ImDrawList* draw_list = ImGui::GetWindowDrawList();

	void getInput();
	void stackUpdate();
	void drawStack(int, std::string[], int, int, float, bool);
	void drawArrow(int, int, int, bool, float);
	bool Push(std::string);
	void Pop();
	void expand();

public:

	Stack(std::string, int&, float&, bool&, int&);
	~Stack();
	void update();

};