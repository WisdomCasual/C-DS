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

	drawWatermark();

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

	updateCam();

	ImGui::End();

	controlsUpdate();
}

Stack::Stack(std::string name, int& state, float& GuiScale, bool& settingsEnabled, int& colorMode)
	: GrandWindow(name, state, GuiScale, settingsEnabled, colorMode)
{

}

Stack::~Stack()
{

}

void Stack::getInput()
{
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

ImU32 Stack::getColor(int color_code)
{
	switch (color_code) {
	case DEFAULT_CELL_COL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(200, 200, 200, 255)) : ImGui::GetColorU32(IM_COL32(150, 150, 150, 255));
	case CELL_BORDER_COL:
		return ImGui::GetColorU32(IM_COL32(40, 40, 40, 255));
	case ARROW1_COL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(200, 50, 50, 255)) : ImGui::GetColorU32(IM_COL32(150, 50, 50, 255));
	case ARROW2_COL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(50, 50, 200, 255)) : ImGui::GetColorU32(IM_COL32(50, 50, 150, 255));
	case END_CELL_COL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(200, 80, 80, 255)) : ImGui::GetColorU32(IM_COL32(150, 50, 50, 255));
	case TEXT_COLOR:
		return colorMode ? ImGui::GetColorU32(IM_COL32(0, 0, 0, 255)) : ImGui::GetColorU32(IM_COL32(255, 255, 255, 255));
	case TEXT_OUTLINE_COL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(255, 255, 255, 255)) : ImGui::GetColorU32(IM_COL32(0, 0, 0, 255));

	default:
		return ImGui::GetColorU32(IM_COL32(255, 0, 255, 255));
	}
}

void Stack::drawText(ImVec2 pos, const char* text)
{
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	for (float x = -1; x <= 1; x++) {
		for (float y = -1; y <= 1; y++) {
			if (x == 0 && y == 0) continue;
			draw_list->AddText(ImVec2(pos.x + x, pos.y + y), getColor(TEXT_OUTLINE_COL), text);
		}
	}

	draw_list->AddText(pos, getColor(TEXT_COLOR), text);
}

void Stack::stackUpdate()
{

	ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);


	float mxCellWidth = STACK_CELL_SIZE * zoomScale;
	for (int i = 0; i < currentMaxSize; i++)
		mxCellWidth = std::max(mxCellWidth, ImGui::CalcTextSize(content[i].c_str()).x + 15.f * zoomScale);

	if (~expansion)
	{
		passedTime += io->DeltaTime * speed;
		if (passedTime >= STACK_BASE_DELAY) {
			expand();
			passedTime = 0.f;
		}

		drawStack(int(center.x - (mxCellWidth+ STACK_CELL_SIZE*zoomScale)/2.f), content, currentMaxSize, headpointer, mxCellWidth, 0);

		if (~expansion)
			drawStack(int(center.x + (mxCellWidth+ STACK_CELL_SIZE*zoomScale)/2.f), tempContent, currentMaxSize * 2, expansion, mxCellWidth, 1);
	}
	else
	{

		if (pending.size()) {
			passedTime += io->DeltaTime * speed;
			if (passedTime >= STACK_BASE_DELAY) {
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

	float separator_size = std::max(STACK_SEPARATOR_SIZE * zoomScale, 1.f);
	float cell_size = STACK_CELL_SIZE * zoomScale;

	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	float ySize = ((cell_size + separator_size) * (mxSz-2));
	float headPointerY = ((cell_size + separator_size) * head) - cell_size / 2.f;

	ImVec2 s_pos(center.x + camPos.x * zoomScale - mxCellWidth / 2.f, center.y + camPos.y * zoomScale + ySize / 2.f);

	if (!inverted)
		s_pos.y += cell_size / 2.f;

	ImVec2 cur_pos(s_pos);

	for (int i = 0; i < mxSz; i++)
	{
		i %= mxSz;
		ImVec2 textSize = ImGui::CalcTextSize(temp[i].c_str());
		ImVec2 pos((cur_pos.x + (mxCellWidth - textSize.x) / 2.f), (cur_pos.y + (cell_size - textSize.y) / 2.f));

		draw_list->AddRectFilled(cur_pos, ImVec2(cur_pos.x + mxCellWidth, cur_pos.y + cell_size), getColor(DEFAULT_CELL_COL), STACK_ROUNDNESS * zoomScale);
		draw_list->AddRect(cur_pos, ImVec2(cur_pos.x + mxCellWidth, cur_pos.y + cell_size), getColor(CELL_BORDER_COL), STACK_ROUNDNESS * zoomScale, 0, 4.f * zoomScale);

		drawText(pos, temp[i].c_str());
		cur_pos.y -= cell_size + separator_size;
	}
	if (!inverted) {
		draw_list->AddRectFilled(cur_pos, ImVec2(cur_pos.x + mxCellWidth, cur_pos.y + cell_size), getColor(END_CELL_COL), STACK_ROUNDNESS * zoomScale);
		draw_list->AddRect(cur_pos, ImVec2(cur_pos.x + mxCellWidth, cur_pos.y + cell_size), getColor(CELL_BORDER_COL), STACK_ROUNDNESS * zoomScale, 0, 4.f * zoomScale);
	}

	drawArrow(int(cur_pos.x), int(s_pos.y - headPointerY), (inverted ? ARROW2_COL : ARROW1_COL), inverted, mxCellWidth);
}

void Stack::drawArrow(int x, int y, int col, bool inverted, float mxCellWidth)
{
	// This will be given the x and y where the head of the arrow should be
	const ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	const float headSize = 20.f * zoomScale;
	const float length = 50.f * zoomScale;

	int sign = 1;
	if (inverted) {
		sign = -1;
		x += int(mxCellWidth);
	}

	ImVec2 from = ImVec2(x - sign * length, (float)y);
	ImVec2 to = ImVec2(x - sign * headSize, (float)y);

	draw_list->AddLine(from, to, getColor(col), 20.f * zoomScale);
	to.x += sign * headSize / 2.f;

	ImVec2 p1 = ImVec2(to.x - sign * headSize, to.y + headSize);
	ImVec2 p2 = ImVec2(to.x - sign * headSize, to.y - headSize);

	draw_list->AddTriangleFilled(p1, p2, to, getColor(col));
}

bool Stack::Push(std::string value)
{
	if (sz == currentMaxSize)
	{
		if (sz == STACK_MAX_SIZE)
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

	if (currentMaxSize * 2 > STACK_MAX_SIZE)
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

	bool noPush = false;
	if (sz == STACK_MAX_SIZE && currentMaxSize >= STACK_MAX_SIZE && !disabled) {
		noPush = true;
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	if (ImGui::Button("Push")) {
		getInput();
	}

	if (noPush) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	bool noPop = false;
	if (sz == 0 && !disabled) {
		noPop = true;
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	if (ImGui::Button("Pop")) {
		Pop();
	}

	if (noPop) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	if (currentMaxSize >= STACK_MAX_SIZE && !disabled) {
		disabled = true;
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	if (ImGui::Button("Expand")) {
		expand();
	}

	if (disabled) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	ImGui::SliderFloat("Speed", &speed, STACK_MIN_SPEED, STACK_MAX_SPEED, "%.1fx", ImGuiSliderFlags_AlwaysClamp);

	ImGui::End();
}