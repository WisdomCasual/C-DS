#include "SparseTable.h"
#include <imgui_internal.h>
#include <iostream>
#include<cmath>


void SparseTable::update()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);
	ImGui::Begin(getName().c_str(), NULL, main_flags);

	ImGui::PopStyleVar();

	drawWatermark();

	//drawtable();
	if(!built)
		drawarray(center.y);

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
	zoomScale += (targetZoom - zoomScale) * 10.f * io->DeltaTime;

	if (ImGui::IsWindowHovered() && io->MouseWheel != 0.0f) {
		targetZoom += io->MouseWheel * 0.15f;
		targetZoom = std::min(std::max(targetZoom, std::min(0.5f, std::min(viewport->WorkSize.x / x_size, viewport->WorkSize.y / y_size) / (CELL_SIZE + SEPARATOR_SIZE))), 3.f);
	}

	ImGui::End();

	controlsUpdate();
}

SparseTable::SparseTable(std::string name, int& state, float& GuiScale, bool& settingsEnabled, int& colorMode)
	: GrandWindow(name, state, GuiScale, settingsEnabled, colorMode)
{
	io = &ImGui::GetIO(); (void)io;
}

SparseTable::~SparseTable()
{

}

void SparseTable::controlsUpdate()
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
	if (ImGui::Button("Reset Camera"))
		camTarget = { 0,0 };
	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));
	ImGui::Text("Insert Element");
	ImGui::InputText(" ", buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_CharsDecimal);
	if (ImGui::Button("Insert")) {
		std::string curElement;
		char* ptr = buffer;
		while (*ptr != '\0') {
			if (*ptr == ',') {
				if (curElement.size() > 0) {
					Elements.push_back(stoi(curElement));
					curElement.clear();
				}
			}
			else
				curElement.push_back(*ptr);

			ptr++;
		}
		if (curElement.size() > 0) {
			Elements.push_back(stoi(curElement));
			curElement.clear();
		}

		memset(buffer, 0, sizeof buffer);
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	if (Elements.size() < 4) {
		disabled = true;
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	ImGui::Dummy(ImVec2(100.0f * GuiScale, 0.0f));
	ImGui::SameLine();
	if (ImGui::Button("Build", ImVec2(200 * GuiScale, ImGui::GetFrameHeight())))
	{	
		build();
	}


	if (disabled) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}
	ImGui::End();

}

ImU32 SparseTable::getColor(int x, int y)
{
	ImU32 col;

	col = ImGui::GetColorU32(IM_COL32(150, 150, 150, 255)); // default

	if (selected[x][y]) {
		col = ImGui::GetColorU32(IM_COL32(255, 87, 51, 255));
	}
	else if (x == cur_cell.x && y == cur_cell.y) {
		col = ImGui::GetColorU32(IM_COL32(100, 100, 100, 255));
	}
	return col;
}

void SparseTable::build()
{

	drawtable();
}

void SparseTable::init()
{
	sz = (int)Elements.size();
	logs.resize(sz + 1);
	table = std::vector<std::vector<int>>(LOG + 1, std::vector<int>(sz));
	for (int i = 2; i <= sz; i++) {
		logs[i] = logs[i >> 1] + 1;
	}
	for(int i=0;i<sz;i++){
		table[0][i] = Elements[i];
	}
	for (int i = 1; i <= logs[sz]; i++) {
		for (int j = 0; j <= sz - (1 << i); j++) {
			table[i][j] = std::min(table[i - 1][j], table[i - 1][j + (1 << (i - 1))]);
		}
	}
}

void SparseTable::drawtable()
{
	init();
	ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);

	float separator_size = std::max(SEPARATOR_SIZE * zoomScale, 1.f);
	float cell_size = CELL_SIZE * zoomScale;

	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	x_size = (int)Elements.size();
	y_size = LOG;
	ImVec2 s_pos(center.x + camPos.x * zoomScale - x_size * (cell_size + separator_size) / 2.f,
		center.y + camPos.y * zoomScale - y_size * (cell_size + separator_size) / 2.f);
	ImVec2 cur_pos(s_pos);
	for (int y = 1; y < y_size; y++) {
		cur_pos.x = s_pos.x;
		for (int x = 0; x < x_size; x++) {

			draw_list->AddRectFilled(cur_pos, ImVec2(cur_pos.x + cell_size, cur_pos.y + cell_size), getColor(y, x));
			cur_pos.x += cell_size + separator_size;
		}
		cur_pos.y += cell_size + separator_size;
	}
}

void SparseTable::drawarray(float yPos)
{
	ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, yPos);

	float separator_size = std::max(SEPARATOR_SIZE * zoomScale, 1.f);
	float cell_size = CELL_SIZE * zoomScale;

	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	sz = (int)Elements.size();
	ImVec2 s_pos(center.x + camPos.x * zoomScale - sz* cell_size / 2.f ,
		center.y + camPos.y * zoomScale - cell_size / 2.f);
	ImVec2 cur_pos(s_pos);
	for (int i = 0; i < sz; i++)
	{
		std::string CurNum;
		int temp = Elements[i];
		while (temp) {
			CurNum.push_back(char((temp % 10) + '0'));
			temp /= 10;
		}
		ImVec2 textSize = ImGui::CalcTextSize(CurNum.c_str());
		ImVec2 pos((cur_pos.x + (std::max(cell_size, textSize.x) - textSize.x) / 2.f), (cur_pos.y + (cell_size - textSize.y) / 2.f));
		ImU32 col = IM_COL32(255, 255, 255, 255);
		draw_list->AddRectFilled(cur_pos, ImVec2(cur_pos.x + std::max(cell_size, textSize.x),
			cur_pos.y + cell_size), IM_COL32(100, 100, 100, 255));
		draw_list->AddText(pos, col, CurNum.c_str());
		cur_pos.x += std::max(cell_size, textSize.x) + separator_size;
	}
}
