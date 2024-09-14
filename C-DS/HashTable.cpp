#include "HashTable.h"
#include <imgui_internal.h>
#include <iostream>

ImU32 HashTable::getColor(int color_code)
{
	switch (color_code) {
	case DEFAULT_BUCKET_COL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(200, 200, 200, 255)) : ImGui::GetColorU32(IM_COL32(150, 150, 150, 255));
	case BUCKET_BORDER_COL:
		return ImGui::GetColorU32(IM_COL32(40, 40, 40, 255));
	case CUR_BUCKET_COL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(80, 200, 80, 255)) : ImGui::GetColorU32(IM_COL32(50, 150, 50, 255));
	case FAIL_BUCKET_COL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(200, 80, 80, 255)) : ImGui::GetColorU32(IM_COL32(150, 50, 50, 255));
	case DEFAULT_NODE_COL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(200, 200, 200, 255)) : ImGui::GetColorU32(IM_COL32(150, 150, 150, 255));
	case ITER_NODE_COL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(50, 200, 200, 255)) : ImGui::GetColorU32(IM_COL32(50, 150, 150, 255));
	case NODE_BORDER_COL:
		return ImGui::GetColorU32(IM_COL32(40, 40, 40, 255));
	case DEFAULT_EDGE_COL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(40, 40, 40, 255)) : ImGui::GetColorU32(IM_COL32(200, 200, 200, 255));
	case FAIL_NODE_COL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(200, 80, 80, 255)) : ImGui::GetColorU32(IM_COL32(150, 50, 50, 255));
	case FOUND_NODE_COL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(80, 200, 80, 255)) : ImGui::GetColorU32(IM_COL32(50, 150, 50, 255));
	case TEXT_COL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(0, 0, 0, 255)) : ImGui::GetColorU32(IM_COL32(255, 255, 255, 255));
	case TEXT_OUTLINE_COL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(255, 255, 255, 255)) : ImGui::GetColorU32(IM_COL32(0, 0, 0, 255));

	default:
		return ImGui::GetColorU32(IM_COL32(255, 0, 255, 255));
	}
}

void HashTable::drawText(ImVec2 pos, const char* text)
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

void HashTable::getInput()
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

void HashTable::selectBucket(int opType)
{
	mode = opType;
	cur_bucket = hashValue(pending.front());
	searchVal = pending.front();
	pending.pop();

	ImVec2 pos(-((table_size + 1) * CELL_SIZE_X + (table_size + 1) * HT_SEPARATOR_SIZE) * 0.5f, CELL_SIZE_Y * 0.5f);
	pos.x += (cur_bucket + 1) * (CELL_SIZE_X + HT_SEPARATOR_SIZE);

	followNode(ImVec2(pos.x, pos.y));

	if (opType == INSERT || opType == FIND) {

		iteratingNode = buckets[cur_bucket];

		if (iteratingNode != nullptr) {
			followNode(iteratingNode->curPos);
			if (iteratingNode->value == searchVal) {
				found = true;
				iteratingNode->color = (opType == INSERT ? FAIL_NODE_COL : FOUND_NODE_COL);
			}
			else
				iteratingNode->color = ITER_NODE_COL;
		}
	}

}

void HashTable::controlsUpdate()
{
	ImVec2 controlsWinSize(std::min(450.f * GuiScale, viewport->WorkSize.x - ImGui::GetStyle().WindowPadding.x), std::min(605.f * GuiScale, viewport->WorkSize.y - 2 * ImGui::GetStyle().WindowPadding.y));
	ImVec2 controlsWinPos(viewport->Size.x - controlsWinSize.x - ImGui::GetStyle().WindowPadding.x, viewport->Size.y - controlsWinSize.y - ImGui::GetStyle().WindowPadding.y);
	bool disabled = false;

	ImGui::SetNextWindowSize(controlsWinSize);
	ImGui::SetNextWindowPos(controlsWinPos);

	ImGui::Begin("Controls", NULL, controls_flags);

	if (ImGui::Button("Back"))
		state = 0;

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	if (ImGui::Button("Reset Camera"))
		camTarget = { 0.0f, 0.0f };

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	if (mode != IDLE || !pending.empty()) {
		disabled = true;
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	ImGui::Text("Initialize Table:");

	ImGui::SliderInt("Table Size", &table_size_slider, MIN_TABLE_SIZE, MAX_TABLE_SIZE, NULL, ImGuiSliderFlags_AlwaysClamp);

	if (ImGui::Button("Create Table")) {

		for (int i = 0; i < table_size; i++) {
			Node* curNode = buckets[i];
			while (curNode != nullptr) {
				Node* delNode = curNode;
				curNode = curNode->next;
				delete delNode;
				delNode = nullptr;
			}
		}

		table_size = table_size_slider;

		buckets = std::vector<Node*>(table_size, nullptr);
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	ImGui::Text("Functions:");

	ImGui::InputText("Value", add_element_content, IM_ARRAYSIZE(add_element_content));

	if (!add_element_content[0] && !disabled) {
		disabled = true;
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	if (ImGui::Button("Insert")) {
		getInput();
		if (!pending.empty())
			selectBucket(INSERT);
	}

	ImGui::SameLine();

	if (ImGui::Button("Find")) {
		getInput();
		if (!pending.empty())
			selectBucket(FIND);
	}

	ImGui::SameLine();

	if (ImGui::Button("Erase")) {
		getInput();
		if (!pending.empty())
			selectBucket(ERASE);
	}

	if (disabled) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	ImGui::Text("Simulation:");

	ImGui::Checkbox("Camera Follow", &camFollow);

	ImGui::SliderFloat("Speed", &speed, HT_MIN_SPEED, HT_MAX_SPEED, "%.1fx", ImGuiSliderFlags_AlwaysClamp);

	ImGui::End();

}

void HashTable::tableUpdate()
{

	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	ImVec2 s_pos(center.x + camPos.x * zoomScale - ((CELL_SIZE_X + HT_SEPARATOR_SIZE) * table_size - HT_SEPARATOR_SIZE) * zoomScale * 0.5f, center.y + camPos.y * zoomScale - CELL_SIZE_Y * zoomScale);
	ImVec2 cur_pos(s_pos);

	ImVec2 targetPos(-((table_size + 1) * CELL_SIZE_X + (table_size + 1) * HT_SEPARATOR_SIZE) * 0.5f, CELL_SIZE_Y * 0.5f);

	for (int i = 0; i < table_size; i++) {

		targetPos.x += CELL_SIZE_X + HT_SEPARATOR_SIZE;
		targetPos.y = -CELL_SIZE_Y * 0.5f;

		if (tempNode != nullptr) {
			if(tempNode->next != nullptr)
			drawEdge(tempNode->curPos, tempNode->next->curPos);

			ImVec2 textCenter = ImGui::CalcTextSize(tempNode->value.c_str());
			textCenter.x /= 2.f;
			textCenter.y /= 2.f;

			draw_list->AddCircleFilled(ImVec2(center.x + (camPos.x + tempNode->curPos.x) * zoomScale, center.y + (camPos.y + tempNode->curPos.y) * zoomScale), HT_VERTEX_RADIUS * zoomScale, getColor(tempNode->color));
			draw_list->AddCircle(ImVec2(center.x + (camPos.x + tempNode->curPos.x) * zoomScale, center.y + (camPos.y + tempNode->curPos.y) * zoomScale), HT_VERTEX_RADIUS * zoomScale, getColor(NODE_BORDER_COL), 100, 5.f * zoomScale);

			
			drawText(ImVec2(center.x + (camPos.x + tempNode->curPos.x) * zoomScale - textCenter.x, center.y + (camPos.y + tempNode->curPos.y) * zoomScale - textCenter.y), tempNode->value.c_str());
		}

		ImVec2 prevPos(targetPos);

		Node* cur_node = buckets[i];

		while (cur_node != nullptr) {
			drawEdge(ImVec2(prevPos. x, prevPos.y), ImVec2(cur_node->curPos.x, cur_node->curPos.y));
			if(cur_node->value == toDelete)
				targetPos.x += HT_NODES_DIST * zoomScale * 0.5f / zoomScale;

			targetPos.y += HT_NODES_DIST * zoomScale / zoomScale;

			ImVec2 textCenter = ImGui::CalcTextSize(cur_node->value.c_str());
			textCenter.x /= 2.f;
			textCenter.y /= 2.f;

			draw_list->AddCircleFilled(ImVec2(center.x + (camPos.x + cur_node->curPos.x) * zoomScale, center.y + (camPos.y + cur_node->curPos.y) * zoomScale), HT_VERTEX_RADIUS * zoomScale, getColor(cur_node->color));
			draw_list->AddCircle(ImVec2(center.x + (camPos.x + cur_node->curPos.x) * zoomScale, center.y + (camPos.y + cur_node->curPos.y) * zoomScale), HT_VERTEX_RADIUS * zoomScale, getColor(NODE_BORDER_COL), 100, 5.f * zoomScale);

			drawText(ImVec2(center.x + (camPos.x + cur_node->curPos.x) * zoomScale - textCenter.x, center.y + (camPos.y + cur_node->curPos.y) * zoomScale - textCenter.y), cur_node->value.c_str());

			cur_node->curPos.x += (targetPos.x - cur_node->curPos.x) * 10.f * io->DeltaTime;
			cur_node->curPos.y += (targetPos.y - cur_node->curPos.y) * 10.f * io->DeltaTime;

			if (cur_node->value == toDelete) {
				targetPos.x -= HT_NODES_DIST * 0.5f;
				targetPos.y -= HT_NODES_DIST;
			}

			prevPos = cur_node->curPos;
			cur_node = cur_node->next;
		}

		std::string bucket_label = std::to_string(i);
		ImVec2 textSize = ImGui::CalcTextSize(bucket_label.c_str());
		ImVec2 bucketPos((cur_pos.x + (CELL_SIZE_X * zoomScale - textSize.x) * 0.5f), (cur_pos.y + (CELL_SIZE_Y * zoomScale - textSize.y) * 0.5f));

		draw_list->AddRectFilled(cur_pos, ImVec2(cur_pos.x + std::max(CELL_SIZE_X * zoomScale, textSize.x), cur_pos.y + CELL_SIZE_Y * zoomScale), getColor(i == cur_bucket ? (buckets[cur_bucket] == nullptr && mode != INSERT ? FAIL_BUCKET_COL : CUR_BUCKET_COL) : DEFAULT_BUCKET_COL), BUCKET_ROUNDNESS * zoomScale);
		draw_list->AddRect(cur_pos, ImVec2(cur_pos.x + std::max(CELL_SIZE_X * zoomScale, textSize.x), cur_pos.y + CELL_SIZE_Y * zoomScale), getColor(BUCKET_BORDER_COL), BUCKET_ROUNDNESS * zoomScale, 0, 5.f * zoomScale);
		
		drawText(bucketPos, bucket_label.c_str());
		cur_pos.x += std::max(CELL_SIZE_X * zoomScale, textSize.x) + HT_SEPARATOR_SIZE * zoomScale;
	}

}

float HashTable::calcDist(float x1, float y1, float x2, float y2)
{
	return sqrtf((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void HashTable::drawEdge(ImVec2 u, ImVec2 v) {

	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	ImVec2 from = ImVec2(center.x + (camPos.x + u.x) * zoomScale, center.y + (camPos.y + u.y) * zoomScale);
	ImVec2 to = ImVec2(center.x + (camPos.x + v.x) * zoomScale, center.y + (camPos.y + v.y) * zoomScale);

	const float thickness = 5.f * zoomScale;

	ImVec2 dir = ImVec2(to.x - from.x, to.y - from.y);
	float length = calcDist(from.x, from.y, to.x, to.y);
	dir.x /= length;
	dir.y /= length;

	const float headSize = 15.f * zoomScale;
	to.x -= dir.x * HT_VERTEX_RADIUS * zoomScale;
	to.y -= dir.y * HT_VERTEX_RADIUS * zoomScale;
	from.x += dir.x * HT_VERTEX_RADIUS * zoomScale;
	from.y += dir.y * HT_VERTEX_RADIUS * zoomScale;

	ImVec2 p1 = ImVec2(to.x - dir.x * headSize - dir.y * headSize, to.y - dir.y * headSize + dir.x * headSize);
	ImVec2 p2 = ImVec2(to.x - dir.x * headSize + dir.y * headSize, to.y - dir.y * headSize - dir.x * headSize);
	draw_list->AddTriangleFilled(p1, p2, to, getColor(DEFAULT_EDGE_COL));

	to.x -= dir.x * headSize / 2;
	to.y -= dir.y * headSize / 2;
	from.x += dir.x * headSize / 2;
	from.y += dir.y * headSize / 2;

	draw_list->AddLine(from, to, getColor(DEFAULT_EDGE_COL), thickness);

}

void HashTable::followNode(ImVec2 pos)
{
	if (!camFollow)
		return;
	camTarget = ImVec2(-pos.x, -pos.y + window_height * 0.3f / zoomScale);
}

void HashTable::insertUpdate()
{
	if (cur_bucket == -1) {
		if (pending.empty())
			mode = IDLE;
		else
			selectBucket(mode);
		return;
	}

	if (buckets[cur_bucket] == nullptr) {
		ImVec2 pos(-((table_size + 1) * CELL_SIZE_X + (table_size + 1) * HT_SEPARATOR_SIZE) * 0.5f, CELL_SIZE_Y * 0.5f);
		pos.x += (cur_bucket + 1) * (CELL_SIZE_X + HT_SEPARATOR_SIZE);
		iteratingNode = buckets[cur_bucket] = new Node(searchVal, pos);
		followNode(ImVec2(iteratingNode->curPos.x, iteratingNode->curPos.y + HT_NODES_DIST));
		iteratingNode->color = FOUND_NODE_COL;
		found = true;
	}
	else if (!found) {
		iteratingNode->color = DEFAULT_NODE_COL;

		if (iteratingNode->next == nullptr) {
			iteratingNode = iteratingNode->next = new Node(searchVal, iteratingNode->curPos);
			iteratingNode->color = FOUND_NODE_COL;
			followNode(ImVec2(iteratingNode->curPos.x, iteratingNode->curPos.y + HT_NODES_DIST));
			found = true;
			return;
		}

		iteratingNode = iteratingNode->next;

		followNode(iteratingNode->curPos);

		if (iteratingNode->value == searchVal) {
			found = true;
			iteratingNode->color = FAIL_NODE_COL;
		}
		else
			iteratingNode->color = ITER_NODE_COL;
	}
	else {
		if (iteratingNode != nullptr)
			iteratingNode->color = DEFAULT_NODE_COL;
		cur_bucket = -1;
		iteratingNode = nullptr;
		found = false;
	}
}

void HashTable::findUpdate()
{
	if (cur_bucket == -1) {
		if (pending.empty())
			mode = IDLE;
		else
			selectBucket(mode);
		return;
	}

	if (iteratingNode != nullptr && !found) {
		iteratingNode->color = DEFAULT_NODE_COL;
		iteratingNode = iteratingNode->next;

		if (iteratingNode != nullptr) {
			followNode(iteratingNode->curPos);
			if (iteratingNode->value == searchVal) {
				found = true;
				iteratingNode->color = FOUND_NODE_COL;
			}
			else
				iteratingNode->color = ITER_NODE_COL;
		}
	}
	else {
		if (iteratingNode != nullptr) {
			followNode(iteratingNode->curPos);
			iteratingNode->color = DEFAULT_NODE_COL;
		}
		iteratingNode = nullptr;
		found = false;
		cur_bucket = -1;
	}
}

void HashTable::eraseUpdate()
{
	if (cur_bucket == -1) {
		if (pending.empty())
			mode = IDLE;
		else
			selectBucket(mode);
		return;
	}

	if (tempNode != nullptr) {
		if (iteratingNode != nullptr) {
			iteratingNode->color = DEFAULT_NODE_COL;
			iteratingNode = nullptr;
		}
		toDelete.clear();
		delete tempNode;
		tempNode = nullptr;
		cur_bucket = -1;
	}
	else if (iteratingNode == nullptr) {

		if (buckets[cur_bucket] == nullptr) {
			cur_bucket = -1;
		}
		else if (buckets[cur_bucket]->value == searchVal) {
			if (toDelete.empty()) {
				toDelete = buckets[cur_bucket]->value;
				followNode(buckets[cur_bucket]->curPos);
			}
			else {
				tempNode = buckets[cur_bucket];
				tempNode->color = FAIL_NODE_COL;
				buckets[cur_bucket] = buckets[cur_bucket]->next;
			}
		}
		else {
			iteratingNode = buckets[cur_bucket];
			followNode(iteratingNode->curPos);
			iteratingNode->color = ITER_NODE_COL;
		}
	}
	else {
		if (iteratingNode->next != nullptr && iteratingNode->next->value != searchVal) {
			iteratingNode->color = DEFAULT_NODE_COL;
			iteratingNode = iteratingNode->next;
			followNode(iteratingNode->curPos);
			iteratingNode->color = ITER_NODE_COL;
		}
		else if (iteratingNode->next != nullptr) {
			if (toDelete.empty()) {
				toDelete = iteratingNode->next->value;
				followNode(iteratingNode->next->curPos);
			}
			else {
				tempNode = iteratingNode->next;
				tempNode->color = FAIL_NODE_COL;
				iteratingNode->next = tempNode->next;
			}
		}
		else {
			if(iteratingNode != nullptr)
				iteratingNode->color = DEFAULT_NODE_COL;
			iteratingNode = nullptr;
			cur_bucket = -1;
		}

	}
}

int HashTable::hashValue(std::string value)
{
	int hash = 0;

	for (int i = 0; i < value.size(); i++) {
		hash = (hash * 257) % table_size;
		hash = (hash + value[i]) % table_size;
	}

	return hash;
}

HashTable::HashTable(std::string name, int& state, float& GuiScale, bool& settingsEnabled, int& colorMode)
	: GrandWindow(name, state, GuiScale, settingsEnabled, colorMode)
{
	buckets = std::vector<Node*>(table_size, nullptr);

	center_y_percent = 0.2f;
}

HashTable::~HashTable()
{

}

void HashTable::update()
{

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);

	ImGui::Begin(getName().c_str(), NULL, main_flags);

	ImGui::PopStyleVar();

	updateCenter();
	window_height = ImGui::GetWindowSize().y;

	drawWatermark();

	tableUpdate();


	if (mode != IDLE) {

		curTime += io->DeltaTime;

		while (curTime * speed >= HT_DELAY) {
			curTime -= HT_DELAY / speed;

			if (mode == INSERT)
				insertUpdate();
			else if (mode == FIND)
				findUpdate();
			else if (mode == ERASE)
				eraseUpdate();
		}
	}

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
