#include "Vector.h"
#include <imgui_internal.h>
#include <iostream>

void Vector::update()
{

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);

	ImGui::Begin(getName().c_str(), NULL, main_flags);

	ImGui::PopStyleVar();

	vectorUpdate();

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

Vector::Vector(std::string name, int& state, float& GuiScale, bool& settingsEnabled, int& colorMode)
	: GrandWindow(name, state, GuiScale, settingsEnabled, colorMode)
{
	io = &ImGui::GetIO(); (void)io;
}

Vector::~Vector()
{

}

ImU32 Vector::getColor(int color_code)
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
	case MARKED_CELL_COL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(80, 200, 200, 255)) : ImGui::GetColorU32(IM_COL32(50, 150, 150, 255));
	case TEXT_COLOR:
		return colorMode ? ImGui::GetColorU32(IM_COL32(0, 0, 0, 255)) : ImGui::GetColorU32(IM_COL32(255, 255, 255, 255));

	default:
		return ImGui::GetColorU32(IM_COL32(255, 0, 255, 255));
	}
}

void Vector::vectorUpdate()
{

	ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);


	if (~insertIdx) {
		passedTime += io->DeltaTime * speed;
		if (passedTime >= BASE_DELAY) {
			insert();
			passedTime = 0.f;
		}
		drawVector((int)center.y, content, currentMaxSize, tailpointer, 0);
		return;
	}

	if (~eraseIdx) {
		passedTime += io->DeltaTime * speed;
		if (passedTime >= BASE_DELAY) {
			erase();
			passedTime = 0.f;
		}

	}

	if (~expansion)
	{
		passedTime += io->DeltaTime * speed;
		if (passedTime >= BASE_DELAY) {
			expand();
			passedTime = 0.f;
		}

		drawVector(int(center.y - VEC_CELL_SIZE * zoomScale), content, currentMaxSize, tailpointer, 0);
		if (~expansion)
			drawVector(int(center.y + VEC_CELL_SIZE * zoomScale), tempContent, currentMaxSize * 2, expansion, 1);
	}
	else
	{
		if (pending.size()) {
			passedTime += io->DeltaTime * speed;
			if (passedTime >= BASE_DELAY) {
				if (pushBack(pending.front())) {
					if (inserting) {
						insert();
						selected_index++;
					}
					pending.pop();
				}
				passedTime = 0.f;
			}
		}
		else
			inserting = 0;

		drawVector((int)center.y, content, currentMaxSize, tailpointer, 0);
	}

}

void Vector::drawVector(int ypos, std::string temp[], int mxSz, int tail, bool inverted)
{

	ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, (float)ypos);

	float separator_size = std::max(VEC_SEPARATOR_SIZE * zoomScale, 1.f);
	float cell_size = VEC_CELL_SIZE * zoomScale;

	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	float xSize = 0.f, tailPointerX = 0.f;

	for (int i = 0; i < mxSz; i++)
	{
		float textWidth = ImGui::CalcTextSize(temp[i].c_str()).x + 15.f * zoomScale;
		xSize += std::max(cell_size, textWidth) + separator_size;

		if (i <= tail) {
			if (i == tail)
				tailPointerX += std::max(cell_size, textWidth) / 2.f;
			else
				tailPointerX += std::max(cell_size, textWidth) + separator_size;
		}
	}

	if (mxSz == tail)
		tailPointerX += cell_size / 2.f;

	ImVec2 s_pos(center.x + camPos.x * zoomScale - xSize / 2.f,
		center.y + camPos.y * zoomScale - cell_size / 2.f);
	if (!inverted)
		s_pos.x -= cell_size / 2.f;

	ImVec2 cur_pos(s_pos);

	for (int i = 0; i < mxSz; i++)
	{
		ImVec2 textSize = ImGui::CalcTextSize(temp[i].c_str());

		ImVec2 pos((cur_pos.x + (std::max(cell_size, textSize.x + 15.f * zoomScale) - textSize.x) / 2.f), (cur_pos.y + (cell_size - textSize.y) / 2.f));

		draw_list->AddRectFilled(cur_pos, ImVec2(cur_pos.x + std::max(cell_size, textSize.x + 15.f * zoomScale), cur_pos.y + cell_size), getColor((i == eraseIdx || i == insertIdx ? MARKED_CELL_COL : DEFAULT_CELL_COL)), VEC_ROUNDNESS * zoomScale);
		draw_list->AddRect(cur_pos, ImVec2(cur_pos.x + std::max(cell_size, textSize.x + 15.f * zoomScale), cur_pos.y + cell_size), getColor(CELL_BORDER_COL), VEC_ROUNDNESS * zoomScale, 0, 4.f * zoomScale);

		draw_list->AddText(pos, getColor(TEXT_COLOR), temp[i].c_str());
		cur_pos.x += std::max(cell_size, textSize.x + 15.f * zoomScale) + separator_size;
	}

	if (!inverted) {
		draw_list->AddRectFilled(cur_pos, ImVec2(cur_pos.x + cell_size, cur_pos.y + cell_size), getColor(END_CELL_COL), VEC_ROUNDNESS * zoomScale);
		draw_list->AddRect(cur_pos, ImVec2(cur_pos.x + cell_size, cur_pos.y + cell_size), getColor(CELL_BORDER_COL), VEC_ROUNDNESS * zoomScale, 0, 4.f * zoomScale);
	}

	drawArrow(int(s_pos.x + tailPointerX), int(cur_pos.y), (inverted ? ARROW2_COL : ARROW1_COL), inverted);

}

void Vector::getInput()
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

void Vector::drawArrow(int x, int y, int col, bool DownToUp)
{
	// This will be given the x and y where the head of the arrow should be
	const ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	const float headSize = 20.f * zoomScale;
	const float length = 50.f * zoomScale;

	int sign = -1;
	if (DownToUp) {
		sign = 1;
		y += int(VEC_CELL_SIZE * zoomScale);
	}
	ImVec2 from = ImVec2((float)x, y + sign * length);
	ImVec2 to = ImVec2((float)x, y + sign * headSize);

	draw_list->AddLine(from, to, getColor(col), 20.f * zoomScale);
	to.y += -sign * headSize / 2.f;

	ImVec2 p1 = ImVec2(to.x + headSize, to.y + sign * headSize);
	ImVec2 p2 = ImVec2(to.x - headSize, to.y + sign * headSize);

	draw_list->AddTriangleFilled(p1, p2, to, getColor(col));
}

bool Vector::pushBack(std::string value)
{
	if (sz == currentMaxSize)
	{
		if (sz == MAX_SIZE)
			return true;

		expand();
		return false;
	}

	content[tailpointer] = value;

	tailpointer++;
	sz++;

	return true;
}

void Vector::expand()
{

	if (currentMaxSize * 2 > MAX_SIZE)
		return;


	if (tempContent == nullptr) {
		tempContent = new std::string[currentMaxSize * 2];
		expansion = 0;
		tailpointer = 0;
		return;
	}

	if (expansion < sz) {
		tempContent[expansion] = content[tailpointer];
		tailpointer++;
		expansion++;
		return;
	}

	tailpointer = expansion;
	// head for popping , tail for pushing 
	delete[] content;
	content = tempContent;
	tempContent = nullptr;
	expansion = -1;
	currentMaxSize *= 2;
}

void Vector::erase()
{
	if (eraseIdx == -1) {
		eraseIdx = selected_index;
		return;
	}

	if (eraseIdx < sz-1) {
		std::swap(content[eraseIdx], content[eraseIdx+1]);
		eraseIdx++;
		return;
	}
	popBack();
	eraseIdx = -1;
}

void Vector::insert()
{
	if (insertIdx == -1) {
		insertIdx = sz - 1;
		return;
	}

	if (insertIdx > selected_index) {
		swap(content[insertIdx], content[insertIdx - 1]);
		insertIdx--;
		return;
	}

	insertIdx = -1;
}

void Vector::popBack()
{
	if (sz == 0)
		return;
	sz--;
	content[--tailpointer] = "";
}

void Vector::updateMenuBar()
{
}

void Vector::controlsUpdate()
{

	ImVec2 controlsWinSize(std::min(450.f * GuiScale, viewport->WorkSize.x - ImGui::GetStyle().WindowPadding.x), std::min(540.f * GuiScale, viewport->WorkSize.y - 2 * ImGui::GetStyle().WindowPadding.y));
	ImVec2 controlsWinPos(viewport->Size.x - controlsWinSize.x - ImGui::GetStyle().WindowPadding.x, viewport->Size.y - controlsWinSize.y - ImGui::GetStyle().WindowPadding.y);

	ImGui::SetNextWindowSize(controlsWinSize);
	ImGui::SetNextWindowPos(controlsWinPos);

	ImGui::Begin("Controls", NULL, controls_flags);

	if (ImGui::Button("Back"))
		state = 0;

	ImGui::Dummy(ImVec2(0.0f, 15.0f * GuiScale));

	if (ImGui::Button("Reset Camera"))
		camTarget = { 0, 0 };

	ImGui::Dummy(ImVec2(0.0f, 15.0f * GuiScale));

	bool disabled = false;
	if (~expansion || pending.size() || ~eraseIdx) {
		disabled = true;
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	ImGui::InputText("Input", add_element_content, IM_ARRAYSIZE(add_element_content));

	ImGui::DragInt("Index", &selected_index, 0.015f, 0, sz, "%d", ImGuiSliderFlags_AlwaysClamp | ((sz == 0) ? ImGuiSliderFlags_ReadOnly : 0));
	selected_index = std::max(0, std::min(selected_index, sz));


	if (ImGui::Button("Push Back")) {
		getInput();
	}

	bool noPush = false;
	if (sz == MAX_SIZE && currentMaxSize >= MAX_SIZE && !disabled) {
		noPush = true;
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	if (ImGui::Button("Insert at Index"))
	{
		getInput();
		inserting = 1;
	}

	if (noPush) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	bool noPop = false;
	if (sz == 0 && !disabled) {
		noPop = true;
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	if (ImGui::Button("Pop Back")) {
		popBack();
	}

	if (selected_index == sz && !disabled && !noPop) {
		noPop = true;
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	if (ImGui::Button("Erase at Index")) {
		erase();
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