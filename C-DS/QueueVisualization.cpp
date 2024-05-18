#include "QueueVisualization.h"
#include <imgui_internal.h>
#include <iostream>
void QueueVisualization::update()
{

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);

	ImGui::Begin(getName().c_str(), NULL, main_flags);

	ImGui::PopStyleVar();

	queueUpdate();

	if ((ImGui::IsWindowHovered() || movingCam) && ((ImGui::IsMouseDown(0) && cur_tool == 0) || ImGui::IsMouseDown(2))) {
		movingCam = true;
		camPos.x += io->MouseDelta.x / zoomScale;
		camPos.y += io->MouseDelta.y / zoomScale;
		camTarget.x += io->MouseDelta.x / zoomScale;
		camTarget.y += io->MouseDelta.y / zoomScale;
	}
	else if (!(ImGui::IsMouseDown(0) && cur_tool == 0) && !ImGui::IsMouseDown(2)) {
		movingCam = false;
	}

	camPos.x += (camTarget.x - camPos.x) * 10.f * io->DeltaTime;
	camPos.y += (camTarget.y - camPos.y) * 10.f * io->DeltaTime;

	if (ImGui::IsWindowHovered() && io->MouseWheel != 0.0f) {
		zoomScale += io->MouseWheel * 0.15f;
		zoomScale = std::min(std::max(zoomScale, std::min(0.5f, std::min(viewport->WorkSize.x / currentMaxSize, viewport->WorkSize.y) / (CELL_SIZE + SEPARATOR_SIZE))), 3.f);
	}

	ImGui::End();

	controlsUpdate();
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

void QueueVisualization::queueUpdate()
{

	ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);

	if (expansion)
	{
		drawQueue(center.y - 30.f * zoomScale, content);
		drawQueue(center.y + 30.f * zoomScale, content);
		expansion = 0;
	}
	else
	{
		drawQueue(center.y, content);
	}

}

void QueueVisualization::drawQueue(int ypos, std::string temp[])
{

	ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, ypos);

	float separator_size = std::max(SEPARATOR_SIZE * zoomScale, 1.f);
	float cell_size = CELL_SIZE * zoomScale;

	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	float xSize = 0.f;

	for (int i = 0; i < currentMaxSize; i++)
		xSize += std::max(cell_size, ImGui::CalcTextSize(content[i].c_str()).x) + separator_size;

	ImVec2 s_pos(center.x + camPos.x * zoomScale - xSize / 2.f,
		center.y + camPos.y * zoomScale - 1 * (cell_size + separator_size) / 2.f);
	ImVec2 cur_pos(s_pos);

	for (int i = 0; i < currentMaxSize; i++)
	{
		i %= currentMaxSize;
		ImVec2 textSize = ImGui::CalcTextSize(content[i].c_str());
		ImVec2 pos((cur_pos.x + (std::max(cell_size, textSize.x) - textSize.x) / 2.f), (cur_pos.y + (cell_size - textSize.y) / 2.f));
		ImU32 col = IM_COL32(255, 255, 255, 255);
		draw_list->AddRectFilled(cur_pos, ImVec2(cur_pos.x + std::max(cell_size, textSize.x),
			cur_pos.y + cell_size), getColor(3));
		draw_list->AddText(pos, col, content[i].c_str());
		cur_pos.x += std::max(cell_size, textSize.x) + separator_size;
	}

}

void QueueVisualization::Enqueue(std::string value)
{
	if (sz == currentMaxSize)
	{
		if (sz == MAX_SIZE)
			return;

		expand();
		content[tailpointer++] = value;
		sz++;
		return;
	}

	content[tailpointer] = value;

	tailpointer++;
	tailpointer %= currentMaxSize;
	sz++;

}
void QueueVisualization::expand()
{
	
	if (currentMaxSize * 2 >= MAX_SIZE)
		return;
	std::string* temp = new std::string[currentMaxSize*2];
	int i = 0;
	for (; i < sz; headpointer++, i++) {
		headpointer %= currentMaxSize;
		temp[i] = content[headpointer]; 
	}
	headpointer = 0, tailpointer = i;
	// head for popping , tail for pushing 
	delete[] content;
	content = temp;
	currentMaxSize *= 2;

}

void QueueVisualization::init()
{

}

void QueueVisualization::Dequeue()
{
	if (sz == 0)
		return;
	sz--;
	content[headpointer++] = "";
	headpointer %= currentMaxSize;
}

void QueueVisualization::updateMenuBar()
{
}

void QueueVisualization::controlsUpdate()
{

	ImVec2 controlsWinSize(std::min(450.f * GuiScale, viewport->WorkSize.x - ImGui::GetStyle().WindowPadding.x), std::min(855.f * GuiScale, viewport->WorkSize.y - 2 * ImGui::GetStyle().WindowPadding.y));
	ImVec2 controlsWinPos(viewport->Size.x - controlsWinSize.x - ImGui::GetStyle().WindowPadding.x, viewport->Size.y - controlsWinSize.y - ImGui::GetStyle().WindowPadding.y);
	bool disabled = false;

	ImGui::SetNextWindowSize(controlsWinSize);
	ImGui::SetNextWindowPos(controlsWinPos);

	ImGui::Begin("Controls", NULL, controls_flags);

	if (ImGui::Button("Back"))
		state = 0;

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	ImGui::Text("Tools: ");

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	ImGui::InputText("##Enter_node", add_element_content, IM_ARRAYSIZE(add_element_content));

	ImGui::SameLine();

	if (ImGui::Button("Enqueue")) {
		std::string curElement;
		char* ptr = add_element_content;
		while (*ptr != '\0') {
			if (*ptr == ',') {
				if (curElement.size() > 0) {
					Enqueue(curElement);
					curElement.clear();
				}
			}
			else
				curElement.push_back(*ptr);

			ptr++;
		}
		if (curElement.size() > 0) {
			Enqueue(curElement);
			curElement.clear();
		}

		memset(add_element_content, 0, sizeof add_element_content);
	}

	if (ImGui::Button("Dequeue"))
	{
		Dequeue();
	}

	if (ImGui::Button("Expand"))
	{
		expand();
		expansion = 1;
	}

	ImGui::End();
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