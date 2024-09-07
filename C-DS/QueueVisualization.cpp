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
	zoomScale += (targetZoom - zoomScale) * 10.f * io->DeltaTime;

	if (ImGui::IsWindowHovered() && io->MouseWheel != 0.0f) {
		targetZoom += io->MouseWheel * 0.15f;
		targetZoom = std::min(std::max(targetZoom, 0.5f), 3.0f);
	}

	ImGui::End();

	controlsUpdate();
}

QueueVisualization::QueueVisualization(std::string name, int& state, float& GuiScale, bool& settingsEnabled, int& colorMode)
	: GrandWindow(name, state, GuiScale, settingsEnabled, colorMode)
{
	io = &ImGui::GetIO(); (void)io;
}

QueueVisualization::~QueueVisualization()
{

}

void QueueVisualization::getInput()
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

ImU32 QueueVisualization::getColor(int color_code)
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
	case TEXT_COLOR:
		return colorMode ? ImGui::GetColorU32(IM_COL32(0, 0, 0, 255)) : ImGui::GetColorU32(IM_COL32(255, 255, 255, 255));
	case TEXT_OUTLINE_COL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(255, 255, 255, 255)) : ImGui::GetColorU32(IM_COL32(0, 0, 0, 255));

	default:
		return ImGui::GetColorU32(IM_COL32(255, 0, 255, 255));
	}
}

void QueueVisualization::drawText(ImVec2 pos, const char* text)
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

void QueueVisualization::queueUpdate()
{

	ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);

	if (~expansion)
	{
		passedTime += io->DeltaTime*speed;
		if (passedTime >= BASE_DELAY) {
			expand();
			passedTime = 0.f;
		}

		drawQueue(int(center.y - 2.f * CELL_SIZE * zoomScale), content, currentMaxSize, tailpointer, headpointer);
		if(~expansion)
			drawQueue(int(center.y + 2.f * CELL_SIZE * zoomScale), tempContent, currentMaxSize*2, expansion, 0);
	}
	else
	{

		if(pending.size()) {
			passedTime += io->DeltaTime * speed;
			if (passedTime >= BASE_DELAY) {
				if(Enqueue(pending.front()))
					pending.pop();
				passedTime = 0.f;
			}
		}

		drawQueue((int)center.y, content, currentMaxSize, tailpointer, headpointer);
	}

}

void QueueVisualization::drawQueue(int ypos, std::string temp[], int mxSz, int tail, int head)
{

	ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, (float)ypos);

	float separator_size = std::max(SEPARATOR_SIZE * zoomScale, 1.f);
	float cell_size = CELL_SIZE * zoomScale;

	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	float xSize = 0.f, headPointerX = 0.f, tailPointerX = 0.f;

	for (int i = 0; i < mxSz; i++)
	{
		float textWidth = ImGui::CalcTextSize(temp[i].c_str()).x + 15.f * zoomScale;
		xSize += std::max(cell_size, textWidth) + separator_size;
		if (i <= head) {
			if (i == head)
				headPointerX += std::max(cell_size, textWidth) / 2.f;
			else
				headPointerX += std::max(cell_size, textWidth) + separator_size;
		}
		if (i <= tail) {
			if (i == tail)
				tailPointerX += std::max(cell_size, textWidth) / 2.f;
			else
				tailPointerX += std::max(cell_size, textWidth) + separator_size;
		}
	}

	ImVec2 s_pos(center.x + camPos.x * zoomScale - xSize / 2.f,
		center.y + camPos.y * zoomScale - cell_size / 2.f);
	ImVec2 cur_pos(s_pos);

	for (int i = 0; i < mxSz; i++)
	{
		i %= mxSz;
		ImVec2 textSize = ImGui::CalcTextSize(temp[i].c_str());
		ImVec2 pos((cur_pos.x + (std::max(cell_size, textSize.x + 15.f * zoomScale) - textSize.x) / 2.f), (cur_pos.y + (cell_size - textSize.y) / 2.f));

		draw_list->AddRectFilled(cur_pos, ImVec2(cur_pos.x + std::max(cell_size, textSize.x + 15.f * zoomScale), cur_pos.y + cell_size), getColor(DEFAULT_CELL_COL), QUEUE_ROUNDNESS * zoomScale);
		draw_list->AddRect(cur_pos, ImVec2(cur_pos.x + std::max(cell_size, textSize.x + 15.f * zoomScale), cur_pos.y + cell_size), getColor(CELL_BORDER_COL), QUEUE_ROUNDNESS * zoomScale, 0, 4.f * zoomScale);
		drawText(pos, temp[i].c_str());
		cur_pos.x += std::max(cell_size, textSize.x + 15.f * zoomScale) + separator_size;
	}

	drawArrow(int(s_pos.x + headPointerX), int(cur_pos.y), ARROW1_COL, 1);
	drawArrow(int(s_pos.x + tailPointerX), int(cur_pos.y), ARROW2_COL, 0);

}

void QueueVisualization::drawArrow(int x, int y, int col, bool DownToUp)
{
	// This will be given the x and y where the head of the arrow should be
	const ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	const float headSize = 20.f * zoomScale;
	const float length = 50.f * zoomScale;

	int sign = -1;
	if(DownToUp) {
		sign = 1;
		y += int(CELL_SIZE*zoomScale);
	}
	ImVec2 from = ImVec2((float)x , y + sign*length);
	ImVec2 to = ImVec2((float)x, y + sign*headSize);

	draw_list->AddLine(from, to, getColor(col), 20.f * zoomScale);
	to.y += -sign*headSize / 2.f;

	ImVec2 p1 = ImVec2(to.x + headSize, to.y + sign*headSize);
	ImVec2 p2 = ImVec2(to.x - headSize, to.y + sign*headSize);

	draw_list->AddTriangleFilled(p1, p2, to, getColor(col));
}

bool QueueVisualization::Enqueue(std::string value)
{
	if (sz == currentMaxSize)
	{
		if (sz == MAX_SIZE)
			return 1;

		expand();
		return 0;
	}

	content[tailpointer] = value;

	tailpointer++;
	tailpointer %= currentMaxSize;
	sz++;

	return 1;
}

void QueueVisualization::expand()
{

	if (currentMaxSize * 2 > MAX_SIZE)
		return;


	if (tempContent == nullptr) {
		tempContent = new std::string[currentMaxSize*2];
		expansion = 0;
		return;
	}

	if(expansion < sz) {
		tempContent[expansion] = content[headpointer];
		headpointer++;
		headpointer %= currentMaxSize;
		expansion++;
		return;
	}

	headpointer = 0, tailpointer = expansion;
	// head for popping , tail for pushing 
	delete[] content;
	content = tempContent;
	tempContent = nullptr;
	expansion = -1;
	currentMaxSize *= 2;
}

void QueueVisualization::Dequeue()
{
	if (sz == 0)
		return;
	sz--;
	content[headpointer++] = "";
	headpointer %= currentMaxSize;
}

void QueueVisualization::controlsUpdate()
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
	if (sz == MAX_SIZE && currentMaxSize >= MAX_SIZE && !disabled) {
		noPush = true;
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	if (ImGui::Button("Enqueue")) {
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

	if (ImGui::Button("Dequeue")) {
		Dequeue();
	}

	if (noPop) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	if (currentMaxSize >= MAX_SIZE && !disabled) {
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

	ImGui::SliderFloat("Speed", &speed, MIN_SPEED, MAX_SPEED, "%.1fx", ImGuiSliderFlags_AlwaysClamp);

	ImGui::End();
}