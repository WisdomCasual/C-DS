#pragma once
#include "GrandWindow.h"
#include <imgui.h>
#include <queue>
#include <string>
class QueueVisualization : public GrandWindow
{
public:
	void update();
	QueueVisualization(std::string, int&, float&, bool&);
	~QueueVisualization();
private:

#define MAX_SIZE 32

	// speed constraints:
#define MAX_SPEED 100.0f
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

	// tools for queue:
	// 1: enqueue
	// 2: dequeue
	// 3: expand (behind the scenes) (for later visualize the the expansion process : ~visualize the 2 contentays~) 
	int cur_tool = 0, sz = 0, tailpointer = 0, headpointer = 0, currentMaxSize = 1, expansion = -1, tempSize = 0;
	bool camfollow = false, movingCam = false;
	ImVec2 camPos = { 0, 0 }, camTarget = { 0, 0 };
	char add_element_content[50] = {};
	float speed = 1.f, passedTime = 0.f;

	std::string* content = new std::string[currentMaxSize], *tempContent = nullptr;
	std::queue<std::string> pending;

	/*
		the insert function works as follows:
		first we need to check for the pointers to know the current state of the queue
		whether it's empty or the average case or it's full (in this case call the expand fucntion)

		the contentay will consist of 3 different colors for the cells
		for the head pointer we'll use blue
		for the tail pointer we'll use red
		for the elements in the middle we'll use grey (what is the pointer in the middle? -- i meant elements sry)

		first in the average case:
		{
			when we insert first we uncolor the current element of the tail pointer
			and then shift the tail pointer and update the contentay with the new value and color the new
			cell red

			when we remove an element we uncolor the current element of the head pointer and then shift
			the head pointer and upadate the contentay the cell with null or some neutral value (tbd later)
			and then shift the the pointer
		}

		// don't forget to check the possibility for making contentows

		in the empty case:
		{
			let's make the assumption that the tail pointer will always point to the currently available cell
			and shift to the next and if the tail pointer is pointing to same index as the head pointer then if
			an enqueue request is made then the expand function is called
		}

		the expand function:
		{

			the expand function works as follows :
			takes the previous contentay size, double it, and then create a new contentay with that size
			iterate from the head pointer to the tail pointer while taking % (the previous contentay size)
			copy that into the new contentay and make the queue pointer point at it, delete the previous contentay

			handle the colors of the head and tail pointers
			handle the camera system and find a suitable scale for each size
			check for the maximum size of expansion (currently at 256)

		}

		*/

	void updateMenuBar();
	void controlsUpdate();
	/*
		operation ids:
		1: enqueue
		2: dequeue
		3: expand (behind the scenes) (for later visualize the expansion process : ~visualize the 2 contentays
		maybe we can have the user expand the queue even if it's not full
	*/
	void useTool(int, int);

	/*
		imu32 col;
		BLUE : col = imgui::getcoloru32(im_col32(50, 50, 150, 255)); (head pointer)
		RED : col = ImGui::GetColorU32(IM_COL32(150, 50, 50, 255)); (tail pointer)
		GREY : col = ImGui::GetColorU32(IM_COL32(150, 150, 150, 255)); (elements in the middle)
		return col;
	*/

	ImU32 getColor(int);

	//ImDrawList* draw_list = ImGui::GetWindowDrawList();

	void queueUpdate();
	void drawQueue(int, std::string[], int, int, int);
	void drawArrow(int, int, int, bool);
	bool Enqueue(std::string);
	void Dequeue();
	void expand();
	void init();
};