#include "Stack.h"
#include <imgui_internal.h>
#include <iostream>

void Stack::update()
{

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);

	ImGui::Begin(getName().c_str(), NULL, main_flags);

	ImGui::PopStyleVar();

	stackUpdate();

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

Stack::Stack(std::string name, int& state, float& scale, bool& settingEnabled)
	: GrandWindow(name, state, scale, settingEnabled)
{
	io = &ImGui::GetIO(); (void)io;
}

Stack::~Stack()
{

}

ImU32 Stack::getColor(int state)
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

void Stack::stackUpdate()
{

	ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);


	float mxCellWidth = CELL_SIZE * zoomScale;
	for (int i = 0; i < currentMaxSize; i++)
	{
		mxCellWidth = std::max(mxCellWidth, ImGui::CalcTextSize(content[i].c_str()).x);
	}

	if (~expansion)
	{
		passedTime += io->DeltaTime * speed;
		if (passedTime >= BASE_DELAY) {
			expand();
			passedTime = 0.f;
		}

		drawStack(int(center.x - (mxCellWidth+CELL_SIZE*zoomScale)/2.f), content, currentMaxSize, headpointer, mxCellWidth, 0);
		if (~expansion)
			drawStack(int(center.x + (mxCellWidth+CELL_SIZE*zoomScale)/2.f), tempContent, currentMaxSize * 2, expansion, mxCellWidth, 1);
	}
	else
	{

		if (pending.size()) {
			passedTime += io->DeltaTime * speed;
			if (passedTime >= BASE_DELAY) {
				if (Push(pending.front()))
					pending.pop();
				passedTime = 0.f;
			}
		}

		drawStack((int)center.x, content, currentMaxSize, headpointer, mxCellWidth, 0);
	}

}

void Stack::drawStack(int xpos, std::string temp[], int mxSz, int head, float mxCellWidth, bool inverted)
{

	ImVec2 center((float)xpos, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);

	float separator_size = std::max(SEPARATOR_SIZE * zoomScale, 1.f);
	float cell_size = CELL_SIZE * zoomScale;

	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	float ySize = ((cell_size+separator_size)*mxSz), headPointerY = ((cell_size + separator_size) * head)-cell_size/2.f;

	ImVec2 s_pos(center.x + camPos.x * zoomScale - mxCellWidth / 2.f,
		center.y + camPos.y * zoomScale + ySize / 2.f);

	if (!inverted)
		s_pos.y += cell_size / 2.f;


	ImVec2 cur_pos(s_pos);

	for (int i = 0; i < mxSz; i++)
	{
		i %= mxSz;
		ImVec2 textSize = ImGui::CalcTextSize(temp[i].c_str());
		ImVec2 pos((cur_pos.x + (mxCellWidth - textSize.x) / 2.f), (cur_pos.y + (cell_size - textSize.y) / 2.f));
		ImU32 col = IM_COL32(255, 255, 255, 255);
		draw_list->AddRectFilled(cur_pos, ImVec2(cur_pos.x + mxCellWidth,
			cur_pos.y + cell_size), getColor(3));
		draw_list->AddText(pos, col, temp[i].c_str());
		cur_pos.y -= cell_size + separator_size;
	}
	if(!inverted)
		draw_list->AddRectFilled(cur_pos, ImVec2(cur_pos.x + mxCellWidth,
			cur_pos.y + cell_size), getColor(2));

	drawArrow(int(cur_pos.x), int(s_pos.y - headPointerY), 1 + (!inverted && mxSz == head), inverted, mxCellWidth);
}

void Stack::drawArrow(int x, int y, int col, bool inverted, float mxCellWidth)
{
	// This will be given the x and y where the head of the arrow should be
	const ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	const float headSize = 12.f * zoomScale;
	const float length = 40.f * zoomScale;

	int sign = 1;
	if (inverted) {
		sign = -1;
		x += int(mxCellWidth);
	}

	ImVec2 from = ImVec2(x - sign * length, (float)y);
	ImVec2 to = ImVec2(x - sign * headSize, (float)y);

	draw_list->AddLine(from, to, getColor(col), 5.f * zoomScale);
	to.x += sign * headSize / 2.f;

	ImVec2 p1 = ImVec2(to.x - sign * headSize, to.y + headSize);
	ImVec2 p2 = ImVec2(to.x - sign * headSize, to.y - headSize);

	draw_list->AddTriangleFilled(p1, p2, to, getColor(col));
}

bool Stack::Push(std::string value)
{
	if (sz == currentMaxSize)
	{
		if (sz == MAX_SIZE)
			return 1;

		expand();
		return 0;
	}

	content[headpointer] = value;

	headpointer++;
	sz++;

	return 1;
}

void Stack::expand()
{

	if (currentMaxSize * 2 > MAX_SIZE)
		return;


	if (tempContent == nullptr) {
		tempContent = new std::string[currentMaxSize * 2];
		expansion = 0;
		headpointer = 0;
		return;
	}

	if (expansion < sz) {
		tempContent[expansion] = content[headpointer];
		headpointer++;
		expansion++;
		return;
	}

	headpointer = sz;
	// head for popping , tail for pushing 
	delete[] content;
	content = tempContent;
	tempContent = nullptr;
	expansion = -1;
	currentMaxSize *= 2;
}


void Stack::Pop()
{
	if (sz == 0)
		return;
	sz--;
	content[--headpointer] = "";
}

void Stack::updateMenuBar()
{

}

void Stack::controlsUpdate()
{

	ImVec2 controlsWinSize(std::min(450.f * GuiScale, viewport->WorkSize.x - ImGui::GetStyle().WindowPadding.x), std::min(340.f * GuiScale, viewport->WorkSize.y - 2 * ImGui::GetStyle().WindowPadding.y));
	ImVec2 controlsWinPos(viewport->Size.x - controlsWinSize.x - ImGui::GetStyle().WindowPadding.x, viewport->Size.y - controlsWinSize.y - ImGui::GetStyle().WindowPadding.y);

	ImGui::SetNextWindowSize(controlsWinSize);
	ImGui::SetNextWindowPos(controlsWinPos);

	ImGui::Begin("Controls", NULL, controls_flags);

	if (ImGui::Button("Back"))
		state = 0;

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	if (ImGui::Button("Reset Camera"))
		camTarget = { 0, 0 };

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	bool disabled = false;
	if (~expansion || pending.size()) {
		disabled = true;
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	ImGui::InputText("##Enter_node", add_element_content, IM_ARRAYSIZE(add_element_content));

	ImGui::SameLine();

	if (ImGui::Button("Push")) {
		std::string curElement;
		char* ptr = add_element_content;
		while (*ptr != '\0') {
			if (*ptr == ',') {
				if (curElement.size() > 0) {
					pending.push(curElement);
					curElement.clear();
				}
			}
			else
				curElement.push_back(*ptr);

			ptr++;
		}
		if (curElement.size() > 0) {
			pending.push(curElement);
			curElement.clear();
		}

		memset(add_element_content, 0, sizeof add_element_content);
	}

	if (ImGui::Button("Pop"))
	{
		Pop();
	}

	if (ImGui::Button("Expand"))
	{
		expand();
	}

	if (disabled) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	ImGui::SliderFloat("Speed", &speed, MIN_SPEED, MAX_SPEED, "%.1fx", ImGuiSliderFlags_AlwaysClamp);

	ImGui::End();
}