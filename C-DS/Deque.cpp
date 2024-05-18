#include "Deque.h"
#include <imgui_internal.h>

void Deque::pushFront()
{
	const ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);

	if (add_node_text[0]) {
		Node* newNode = new Node(add_node_text, (head == nullptr ? ImVec2(0, 0) : head->curPos));
		newNode->next = head;
		if (head != nullptr)
			head->prev = newNode;
		head = newNode;
		if (tail == nullptr)
			tail = newNode;
		if (center.x + (camPos.x + newNode->curPos.x) * zoomScale + EDGE_LENGTH > viewport->WorkSize.x ||
			center.x + (camPos.x + newNode->curPos.x) * zoomScale - EDGE_LENGTH < 0.f)
			followNode(newNode->curPos);
		dequeSize++;
		memset(add_node_text, 0, sizeof add_node_text);
	}
}

void Deque::pushBack()
{
	const ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);

	if (add_node_text[0]) {
		Node* newNode = new Node(add_node_text, (tail == nullptr ? ImVec2(0, 0) : tail->curPos));
		newNode->prev = tail;

		if (tail != nullptr)
			tail->next = newNode;
		tail = newNode;
		if (head == nullptr)
			head = newNode;
		if (center.x + (camPos.x + newNode->curPos.x) * zoomScale + EDGE_LENGTH > viewport->WorkSize.x ||
			center.x + (camPos.x + newNode->curPos.x) * zoomScale - EDGE_LENGTH < 0.f)
			followNode(newNode->curPos);
		dequeSize++;
		memset(add_node_text, 0, sizeof add_node_text);
	}
}

void Deque::popFront()
{
	const ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);

	if (dequeSize != 0) {
		Node* delNode = head;
		head = head->next;
		if(head != nullptr)
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
		if(tail != nullptr)
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

void Deque::controlsUpdate()
{
	const ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);
	ImVec2 controlsWinSize(std::min(450.f * GuiScale, viewport->WorkSize.x - ImGui::GetStyle().WindowPadding.x), std::min(560.f * GuiScale, viewport->WorkSize.y - 2 * ImGui::GetStyle().WindowPadding.y));
	ImVec2 controlsWinPos(viewport->Size.x - controlsWinSize.x - ImGui::GetStyle().WindowPadding.x, viewport->Size.y - controlsWinSize.y - ImGui::GetStyle().WindowPadding.y);
	bool disabled = false;

	ImGui::SetNextWindowSize(controlsWinSize);
	ImGui::SetNextWindowPos(controlsWinPos);

	ImGui::Begin("Controls", NULL, controls_flags);

	if (ImGui::Button("Back"))
		state = 0;

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	if (iterationMode != 0) {
		disabled = true;
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	if (ImGui::Button("Reset Camera"))
		camTarget = { 0, 0 };

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	ImGui::InputText("Value", add_node_text, IM_ARRAYSIZE(add_node_text));

	ImGui::DragInt("Index", &selected_index, 0.015f, 0, dequeSize, "%d", ImGuiSliderFlags_AlwaysClamp | ((dequeSize == 0) ? ImGuiSliderFlags_ReadOnly : 0));
	selected_index = std::max(0, std::min(selected_index, dequeSize));

	bool insertDisabled = false;

	if (add_node_text[0] == 0 && !disabled) {
		insertDisabled = true;
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	if (ImGui::Button("Push Front")) {
		pushFront();
	}

	if (ImGui::Button("Push Back")) {
		pushBack();
	}

	if (ImGui::Button("Push at Index")) {
		if (add_node_text[0]) {
			if (selected_index == 0) {
				pushFront();
			}
			else if (selected_index == dequeSize) {
				pushBack();
			}
			else {
				iterationMode = 1;
				highlighted_idx = 0;
				curNode = head;
				followNode(head->curPos);

				insert_val = add_node_text;
				memset(add_node_text, 0, sizeof add_node_text);
			}
		}
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

	ImGui::SliderFloat("Speed", &speed, LL_MIN_SPEED, LL_MAX_SPEED, "%.1fx", ImGuiSliderFlags_AlwaysClamp);

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
	to.x -= dir.x * VERTEX_RADIUS;
	to.y -= dir.y * VERTEX_RADIUS;
	from.x += dir.x * VERTEX_RADIUS;
	from.y += dir.y * VERTEX_RADIUS;

	ImVec2 p1 = ImVec2(to.x - dir.x * headSize - dir.y * headSize, to.y - dir.y * headSize + dir.x * headSize);
	ImVec2 p2 = ImVec2(to.x - dir.x * headSize + dir.y * headSize, to.y - dir.y * headSize - dir.x * headSize);
	draw_list->AddTriangleFilled(p1, p2, to, DEFAULT_EDGE_COL);

	to.x -= dir.x * headSize / 2;
	to.y -= dir.y * headSize / 2;
	from.x += dir.x * headSize / 2;
	from.y += dir.y * headSize / 2;

	draw_list->AddLine(from, to, DEFAULT_EDGE_COL, thickness);

}

void Deque::dequeUpdate()
{
	const ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	Node* cur_node = head;
	ImVec2 targetPos(-(float)(dequeSize - 1) * EDGE_LENGTH / 2.f, 0.f);
	int curIdx = 0;

	if (tempNode != nullptr) {
		ImVec2 textCenter = ImGui::CalcTextSize(tempNode->value.c_str());
		textCenter.x /= 2.f;
		textCenter.y /= 2.f;

		draw_list->AddCircleFilled(ImVec2(center.x + (camPos.x + tempNode->curPos.x) * zoomScale, center.y + (camPos.y + tempNode->curPos.y) * zoomScale), VERTEX_RADIUS, DEFAULT_NODE_COL);

		if (tempNode->next != nullptr)
			drawEdge(tempNode->curPos, tempNode->next->curPos);

		if (tempNode->prev != nullptr)
			drawEdge(tempNode->curPos, tempNode->prev->curPos);

		draw_list->AddText(ImVec2(center.x + (camPos.x + tempNode->curPos.x) * zoomScale - textCenter.x, center.y + (camPos.y + tempNode->curPos.y) * zoomScale - textCenter.y), TEXT_COL, tempNode->value.c_str());

		tempNode->curPos.y += (-EDGE_LENGTH - tempNode->curPos.y) * 20.f * io->DeltaTime;
	}

	while (cur_node != nullptr) {

		if (cur_node != tempNode) {
			ImVec2 textCenter = ImGui::CalcTextSize(cur_node->value.c_str());
			textCenter.x /= 2.f;
			textCenter.y /= 2.f;

			draw_list->AddCircleFilled(ImVec2(center.x + (camPos.x + cur_node->curPos.x) * zoomScale, center.y + (camPos.y + cur_node->curPos.y) * zoomScale), VERTEX_RADIUS, (curIdx == highlighted_idx ? ITER_VERT_COL : DEFAULT_NODE_COL));

			if (cur_node->next != nullptr)
				drawEdge(cur_node->curPos, cur_node->next->curPos);

			if (cur_node->prev != nullptr)
				drawEdge(cur_node->curPos, cur_node->prev->curPos);

			draw_list->AddText(ImVec2(center.x + (camPos.x + cur_node->curPos.x) * zoomScale - textCenter.x, center.y + (camPos.y + cur_node->curPos.y) * zoomScale - textCenter.y), TEXT_COL, cur_node->value.c_str());

			cur_node->curPos.x += (targetPos.x - cur_node->curPos.x) * 10.f * io->DeltaTime;
			cur_node->curPos.y += (targetPos.y - cur_node->curPos.y) * 10.f * io->DeltaTime;

			targetPos.x += EDGE_LENGTH;
		}
		cur_node = cur_node->next;
		curIdx++;
	}

}

void Deque::followNode(ImVec2 pos)
{
	camTarget = ImVec2(-pos.x, -pos.y);
}

Deque::Deque(std::string name, int& state, float& scale, bool& settingEnabled)
	: GrandWindow(name, state, scale, settingsEnabled)
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

	camPos.x += (camTarget.x - camPos.x) * 10.f * io->DeltaTime;
	camPos.y += (camTarget.y - camPos.y) * 10.f * io->DeltaTime;

	if (ImGui::IsWindowHovered() && io->MouseWheel != 0.0f) {
		zoomScale += io->MouseWheel * 0.15f;
		zoomScale = std::min(std::max(zoomScale, 0.5f), 3.0f);
	}


	if (iterationMode != 0) {
		curTime += io->DeltaTime;

		while (curTime * speed >= LL_DELAY) {

			curTime -= LL_DELAY / speed;

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
						highlighted_idx = -1;
						tempNode = nullptr;
						iterationMode = 0;
					}
				}
			}
			else {
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
						if(curNode->next != nullptr)
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

		}

	}

	ImGui::End();

	controlsUpdate();

}
