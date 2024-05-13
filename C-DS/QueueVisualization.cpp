#include "QueueVisualization.h"
#include <imgui_internal.h>

void QueueVisualization::update()
{
}

QueueVisualization::QueueVisualization(std::string name, int& state, float& scale, bool& settingEnabled)
	: GrandWindow(name, state, scale, settingEnabled)
{
	io = &ImGui::GetIO(); (void)io;
}
QueueVisualization::~QueueVisualization()
{

}
ImU32 QueueVisualization::getColor(int state)
{
	ImU32 col;

	//Cyan : 1, Red : 2, Grey : 3

	switch (state)
	{
	case 1:
		col = ImGui::GetColorU32(IM_COL32(50, 150, 150, 255));
		break;
	case 2:
		col = ImGui::GetColorU32(IM_COL32(150, 50, 50, 255));;
		break;
	default:
		col = ImGui::GetColorU32(IM_COL32(150, 150, 150, 255));
	}

	return col;
}
void QueueVisualization::Enqueue(int value)
{
	if (sz == currentMaxSize)
	{
		if (sz == MAX_SIZE)
			return;

		expand();
		arr[tailpointer++] = value;
		sz++;
		return;
	}

	arr[tailpointer++] = value;
	tailpointer %= currentMaxSize;
	sz++;

}
void QueueVisualization::expand()
{
	std::string* temp = new std::string[currentMaxSize*2];
	int i;
	for (i = 0; headpointer != tailpointer; headpointer++, i++) {
		headpointer %= currentMaxSize;
		temp[i] = arr[headpointer];
	}
	if (headpointer != -1)
		temp[i] = arr[headpointer];
	delete[] arr;
	arr = temp;
	currentMaxSize *= 2;
}

void QueueVisualization::init()
{

}

void QueueVisualization::Dequeue()
{
	if (!sz)
		return;

	sz--;
	arr[headpointer++] = '0';
	headpointer %= currentMaxSize;

}

void QueueVisualization::updateMenuBar()
{
}

void QueueVisualization::controlsUpdate()
{
}

void QueueVisualization::useTool(int toolIndx, int value = -1)
{
	//Enqueue
	if (toolIndx == 1)
	{
		//ImDrawList* draw_list = ImGui::GetWindowDrawList();
		//draw_list->AddRectFilled();
	}
	//Dequeue
	else if (toolIndx == 2)
	{

	}
	//Expand
	else
	{

	}

}