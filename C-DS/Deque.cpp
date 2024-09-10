#include "Deque.h"
#include <imgui_internal.h>

void Deque::pushFront(std::string node)
{
	const ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);

	Node* newNode = new Node(node, (head == nullptr ? ImVec2(0, 0) : head->curPos));
	newNode->next = head;
	if (head != nullptr)
		head->prev = newNode;
	head = newNode;
	if (tail == nullptr)
		tail = newNode;
	if (center.x + (camPos.x + newNode->curPos.x) * zoomScale + DQ_NODES_SPACING > viewport->WorkSize.x ||
		center.x + (camPos.x + newNode->curPos.x) * zoomScale - DQ_NODES_SPACING < 0.f)
		followNode(newNode->curPos);
	dequeSize++;
}

void Deque::pushBack(std::string node)
{
	const ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);

	Node* newNode = new Node(node, (tail == nullptr ? ImVec2(0, 0) : tail->curPos));
	newNode->prev = tail;

	if (tail != nullptr)
		tail->next = newNode;
	tail = newNode;
	if (head == nullptr)
		head = newNode;
	if (center.x + (camPos.x + newNode->curPos.x) * zoomScale + DQ_NODES_SPACING > viewport->WorkSize.x ||
		center.x + (camPos.x + newNode->curPos.x) * zoomScale - DQ_NODES_SPACING < 0.f)
		followNode(newNode->curPos);
	dequeSize++;
}

void Deque::popFront()
{
	const ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);

	if (dequeSize != 0) {
		Node* delNode = head;
		head = head->next;
		if (head != nullptr)
			head->prev = nullptr;
		dequeSize--;
		if (dequeSize == 0)
			tail = nullptr;
		if (center.x + (camPos.x + delNode->curPos.x) * zoomScale > viewport->WorkSize.x ||
			center.x + (camPos.x + delNode->curPos.x) * zoomScale < 0.f)
			followNode(delNode->curPos);
		delete delNode;
	}
}

void Deque::popBack()
{
	const ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);

	if (dequeSize != 0) {

		Node* delNode = tail;
		tail = tail->prev;
		if (tail != nullptr)
			tail->next = nullptr;
		dequeSize--;
		if (dequeSize == 0)
			head = nullptr;
		if (center.x + (camPos.x + delNode->curPos.x) * zoomScale > viewport->WorkSize.x ||
			center.x + (camPos.x + delNode->curPos.x) * zoomScale < 0.f)
			followNode(delNode->curPos);
		delete delNode;
	}
}

ImU32 Deque::getColor(int color_code)
{
	switch (color_code) {
	case DEF_VERT_COL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(200, 200, 200, 255)) : ImGui::GetColorU32(IM_COL32(150, 150, 150, 255));
	case ITER_VERT_COL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(50, 200, 200, 255)) : ImGui::GetColorU32(IM_COL32(50, 150, 150, 255));
	case VERT_BORDER_COL:
		return ImGui::GetColorU32(IM_COL32(40, 40, 40, 255));
	case DEFAULT_EDGE_COL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(40, 40, 40, 255)) : ImGui::GetColorU32(IM_COL32(200, 200, 200, 255));
	case TEXT_COL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(0, 0, 0, 255)) : ImGui::GetColorU32(IM_COL32(255, 255, 255, 255));
	case TEXT_OUTLINE_COL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(255, 255, 255, 255)) : ImGui::GetColorU32(IM_COL32(0, 0, 0, 255));

	default:
		return ImGui::GetColorU32(IM_COL32(255, 0, 255, 255));
	}
}

void Deque::drawText(ImVec2 pos, const char* text)
{
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	for (float x = -1; x <= 1; x++) {
		for (float y = -1; y <= 1; y++) {
			if (x == 0 && y == 0) continue;
			draw_list->AddText(ImVec2(pos.x + x, pos.y + y), getColor(TEXT_OUTLINE_COL), text);
		}
	}

	draw_list->AddText(pos, getColor(TEXT_COL), text);
}

void Deque::getInput()
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

void Deque::controlsUpdate()
{
	const ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);
	ImVec2 controlsWinSize(std::min(450.f * GuiScale, viewport->WorkSize.x - ImGui::GetStyle().WindowPadding.x), std::min(620.f * GuiScale, viewport->WorkSize.y - 2 * ImGui::GetStyle().WindowPadding.y));
	ImVec2 controlsWinPos(viewport->Size.x - controlsWinSize.x - ImGui::GetStyle().WindowPadding.x, viewport->Size.y - controlsWinSize.y - ImGui::GetStyle().WindowPadding.y);
	bool disabled = false;

	ImGui::SetNextWindowSize(controlsWinSize);
	ImGui::SetNextWindowPos(controlsWinPos);

	ImGui::Begin("Controls", NULL, controls_flags);

	if (ImGui::Button("Back"))
		state = 0;

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	if (ImGui::Button("Reset Camera"))
		camTarget = { 0, 0 };

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	ImGui::InputText("Value", add_element_content, IM_ARRAYSIZE(add_element_content));

	if (iterationMode != 0) {
		disabled = true;
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	ImGui::DragInt("Index", &selected_index, 0.015f, 0, dequeSize, "%d", ImGuiSliderFlags_AlwaysClamp | ((dequeSize == 0) ? ImGuiSliderFlags_ReadOnly : 0));
	selected_index = std::max(0, std::min(selected_index, dequeSize));

	bool insertDisabled = false;

	if (add_element_content[0] == 0 && !disabled) {
		insertDisabled = true;
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	if (ImGui::Button("Push Front")) {
		getInput();
		mode = PUSH_FRONT;
		curTime = DQ_DELAY / speed;
	}

	if (ImGui::Button("Push Back")) {
		getInput();
		mode = PUSH_BACK;
		curTime = DQ_DELAY / speed;

	}

	if (ImGui::Button("Push at Index")) {
		getInput();
		mode = INSERT;
		curTime = DQ_DELAY / speed;
	}

	if (insertDisabled) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	if (dequeSize == 0) {
		disabled = true;
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	if (ImGui::Button("Pop Front")) {
		popFront();
	}

	if (ImGui::Button("Pop Back")) {
		popBack();
	}

	bool delIdxValid = true;
	if (selected_index == dequeSize && !disabled) {
		delIdxValid = false;
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	if (ImGui::Button("Pop Index")) {
		if (selected_index < dequeSize) {
			if (selected_index == 0) {
				popFront();
			}
			else if (selected_index == dequeSize - 1) {
				popBack();
			}
			else {
				iterationMode = 2;
				highlighted_idx = 0;
				curNode = head;
				followNode(head->curPos);
			}
		}
	}

	if (!delIdxValid) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	if (disabled) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	ImGui::Checkbox("Camera Follow", &camFollow);

	ImGui::SliderFloat("Speed", &speed, DQ_MIN_SPEED, DQ_MAX_SPEED, "%.1fx", ImGuiSliderFlags_AlwaysClamp);

	ImGui::End();

}

float Deque::calcDist(float x1, float y1, float x2, float y2)
{
	return sqrtf((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void Deque::drawEdge(ImVec2& u, ImVec2& v) {

	ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	ImVec2 from = ImVec2(center.x + (camPos.x + u.x) * zoomScale, center.y + (camPos.y + u.y) * zoomScale);
	ImVec2 to = ImVec2(center.x + (camPos.x + v.x) * zoomScale, center.y + (camPos.y + v.y) * zoomScale);

	const float thickness = 5.f * zoomScale;

	ImVec2 dir = ImVec2(to.x - from.x, to.y - from.y);
	float length = calcDist(from.x, from.y, to.x, to.y);
	dir.x /= length;
	dir.y /= length;

	const float headSize = 15.f * zoomScale;
	to.x -= dir.x * DQ_VERTEX_RADIUS * zoomScale;
	to.y -= dir.y * DQ_VERTEX_RADIUS * zoomScale;
	from.x += dir.x * DQ_VERTEX_RADIUS * zoomScale;
	from.y += dir.y * DQ_VERTEX_RADIUS * zoomScale;

	ImVec2 p1 = ImVec2(to.x - dir.x * headSize - dir.y * headSize, to.y - dir.y * headSize + dir.x * headSize);
	ImVec2 p2 = ImVec2(to.x - dir.x * headSize + dir.y * headSize, to.y - dir.y * headSize - dir.x * headSize);
	draw_list->AddTriangleFilled(p1, p2, to, getColor(DEFAULT_EDGE_COL));

	to.x -= dir.x * headSize / 2;
	to.y -= dir.y * headSize / 2;
	from.x += dir.x * headSize / 2;
	from.y += dir.y * headSize / 2;

	draw_list->AddLine(from, to, getColor(DEFAULT_EDGE_COL), thickness);

}

void Deque::dequeUpdate()
{
	const ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	Node* cur_node = head;
	ImVec2 targetPos(-(float)(dequeSize - 1) * DQ_NODES_SPACING / 2.f, 0.f);
	int curIdx = 0;

	if (tempNode != nullptr) {
		ImVec2 textCenter = ImGui::CalcTextSize(tempNode->value.c_str());
		textCenter.x /= 2.f;
		textCenter.y /= 2.f;

		draw_list->AddCircleFilled(ImVec2(center.x + (camPos.x + tempNode->curPos.x) * zoomScale, center.y + (camPos.y + tempNode->curPos.y) * zoomScale), DQ_VERTEX_RADIUS * zoomScale, getColor(DEF_VERT_COL));
		draw_list->AddCircle(ImVec2(center.x + (camPos.x + tempNode->curPos.x) * zoomScale, center.y + (camPos.y + tempNode->curPos.y) * zoomScale), DQ_VERTEX_RADIUS * zoomScale, getColor(VERT_BORDER_COL), 100, 5.f * zoomScale);


		if (tempNode->next != nullptr)
			drawEdge(tempNode->curPos, tempNode->next->curPos);

		if (tempNode->prev != nullptr)
			drawEdge(tempNode->curPos, tempNode->prev->curPos);

		drawText(ImVec2(center.x + (camPos.x + tempNode->curPos.x) * zoomScale - textCenter.x, center.y + (camPos.y + tempNode->curPos.y) * zoomScale - textCenter.y), tempNode->value.c_str());

		tempNode->curPos.y += (-DQ_NODES_SPACING - tempNode->curPos.y) * 20.f * io->DeltaTime;
	}

	while (cur_node != nullptr) {

		if (cur_node != tempNode) {
			ImVec2 textCenter = ImGui::CalcTextSize(cur_node->value.c_str());
			textCenter.x /= 2.f;
			textCenter.y /= 2.f;

			draw_list->AddCircleFilled(ImVec2(center.x + (camPos.x + cur_node->curPos.x) * zoomScale, center.y + (camPos.y + cur_node->curPos.y) * zoomScale), DQ_VERTEX_RADIUS * zoomScale, getColor(curIdx == highlighted_idx ? ITER_VERT_COL : DEF_VERT_COL));
			draw_list->AddCircle(ImVec2(center.x + (camPos.x + cur_node->curPos.x) * zoomScale, center.y + (camPos.y + cur_node->curPos.y) * zoomScale), DQ_VERTEX_RADIUS * zoomScale, getColor(VERT_BORDER_COL), 100, 5.f * zoomScale);

			if (cur_node->next != nullptr)
				drawEdge(cur_node->curPos, cur_node->next->curPos);

			if (cur_node->prev != nullptr)
				drawEdge(cur_node->curPos, cur_node->prev->curPos);

			drawText(ImVec2(center.x + (camPos.x + cur_node->curPos.x) * zoomScale - textCenter.x, center.y + (camPos.y + cur_node->curPos.y) * zoomScale - textCenter.y), cur_node->value.c_str());

			cur_node->curPos.x += (targetPos.x - cur_node->curPos.x) * 10.f * io->DeltaTime;
			cur_node->curPos.y += (targetPos.y - cur_node->curPos.y) * 10.f * io->DeltaTime;

			targetPos.x += DQ_NODES_SPACING;
		}
		cur_node = cur_node->next;
		curIdx++;
	}

}

void Deque::followNode(ImVec2 pos)
{
	if (!camFollow)
		return;
	camTarget = ImVec2(-pos.x, -pos.y);
}

Deque::Deque(std::string name, int& state, float& GuiScale, bool& settingsEnabled, int& colorMode)
	: GrandWindow(name, state, GuiScale, settingsEnabled, colorMode)
{

}

Deque::~Deque()
{

}

void Deque::update()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);

	ImGui::Begin(getName().c_str(), NULL, main_flags);

	ImGui::PopStyleVar();

	drawWatermark();

	dequeUpdate();

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

	if (iterationMode != 0 || !pending.empty()) {
		curTime += io->DeltaTime;


		while (curTime * speed >= DQ_DELAY) {

			curTime -= DQ_DELAY / speed;

			if (iterationMode == 1) {
				if (highlighted_idx + 1 < selected_index) {
					highlighted_idx++;
					curNode = curNode->next;
					followNode(curNode->curPos);
				}
				else {
					if (tempNode == nullptr) {
						tempNode = new Node(insert_val, curNode->next->curPos);
						tempNode->next = curNode->next;
						tempNode->prev = curNode;
					}
					else {
						curNode->next->prev = tempNode;
						curNode->next = tempNode;
						dequeSize++;
						tempNode = nullptr;
						if (pending.empty()) {
							highlighted_idx = -1;
							curNode = nullptr;
							iterationMode = 0;
						}
						else {
							insert_val = pending.front();
							pending.pop();
							selected_index++;
						}
					}
				}
			}
			else if (iterationMode == 2) {
				if (highlighted_idx + 1 < selected_index) {
					highlighted_idx++;
					curNode = curNode->next;
					followNode(curNode->curPos);
				}
				else {
					if (tempNode == nullptr) {
						tempNode = curNode->next;
						dequeSize--;
					}
					else if (curNode->next == tempNode) {
						curNode->next = tempNode->next;
						if (curNode->next != nullptr)
							curNode->next->prev = curNode;
					}
					else {
						delete tempNode;
						if (selected_index == dequeSize)
							tail = curNode;
						highlighted_idx = -1;
						tempNode = nullptr;
						iterationMode = 0;
					}
				}
			}
			else {

				std::string cur = pending.front();
				pending.pop();

				if (mode == PUSH_BACK)
					pushBack(cur);

				if (mode == PUSH_FRONT)
					pushFront(cur);

				if (mode == INSERT) {
					if (selected_index == 0)
						pushFront(cur), selected_index++;
					else if (selected_index == dequeSize)
						pushBack(cur), selected_index++;
					else {
						iterationMode = 1;
						highlighted_idx = 0;
						curNode = head;
						followNode(curNode->curPos);
						insert_val = cur;
					}
				}
			}
		}
	}

	ImGui::End();
	controlsUpdate();

}
