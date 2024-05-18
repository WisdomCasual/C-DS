#pragma once
#include "GrandWindow.h"
#include <imgui.h>
#include <queue>
#include <string>
class Stack : public GrandWindow
{
public:
	void update();
	Stack(std::string, int&, float&, bool&);
	~Stack();
private:

#define MAX_SIZE 32

	// speed constraints:
#define MAX_SPEED 10.0f
#define MIN_SPEED 0.1f
#define DELAY_TIME 0.2f
#define BASE_DELAY 0.6f

#define CELL_SIZE 70.f
#define SEPARATOR_SIZE 5.f

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

	

	void updateMenuBar();
	void controlsUpdate();
	
	void useTool(int, int);


	ImU32 getColor(int);

	//ImDrawList* draw_list = ImGui::GetWindowDrawList();

	void stackUpdate();
	void drawStack(int, std::string[], int, int, float, bool);
	void drawArrow(int, int, int, bool, float);
	bool Push(std::string);
	void Pop();
	void expand();
	void init();
};