#pragma once
#include "GrandWindow.h"
#include <imgui.h>
#include <queue>
#include <string>
class QueueVisualization : public GrandWindow
{
private:

	const int MAX_SIZE = 1024;

	// speed constraints:
	const float MAX_SPEED = 10.0f;
	const float MIN_SPEED = 0.1f;
	const float DELAY_TIME = 0.2f;
	const float BASE_DELAY = 0.6f;

	const float QUEUE_ROUNDNESS = 10.f;
	const float CELL_SIZE = 70.f;
	const float SEPARATOR_SIZE = 5.f;

	enum {
		DEFAULT_CELL_COL,
		CELL_BORDER_COL,
		ARROW1_COL,
		ARROW2_COL,
		TEXT_COLOR
	};

// private fields:
	ImGuiWindowFlags main_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
	ImGuiWindowFlags controls_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGuiIO* io;

	int cur_tool = 0, sz = 0, tailpointer = 0, headpointer = 0, currentMaxSize = 1, expansion = -1, tempSize = 0;
	bool camfollow = false, movingCam = false;
	ImVec2 camPos = { 0, 0 }, camTarget = { 0, 0 };
	char add_element_content[200] = {};
	float speed = 1.f, passedTime = 0.f;

	std::string* content = new std::string[currentMaxSize], *tempContent = nullptr;
	std::queue<std::string> pending;

	void controlsUpdate();
	void getInput();
	ImU32 getColor(int);
	void queueUpdate();
	void drawQueue(int, std::string[], int, int, int);
	void drawArrow(int, int, int, bool);
	bool Enqueue(std::string);
	void Dequeue();
	void expand();

public:
	QueueVisualization(std::string, int&, float&, bool&, int&);
	~QueueVisualization();
	void update();

};