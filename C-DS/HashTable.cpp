#include "HashTable.h"
#include <imgui_internal.h>
#include <iostream>

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
		camTarget = { 0, 0 };

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	if (mode != 0) {
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

	ImGui::InputText("Value", add_node_text, IM_ARRAYSIZE(add_node_text));

	if (!add_node_text[0] && !disabled) {
		disabled = true;
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	if (ImGui::Button("Insert")) {
		if (add_node_text[0]) {
			cur_bucket = hashValue(add_node_text);
			searchVal = add_node_text;
			memset(add_node_text, 0, sizeof add_node_text);
			mode = 1;

			iteratingNode = buckets[cur_bucket];

			ImVec2 pos(-((table_size + 1) * CELL_SIZE_X + (table_size + 2) * HT_SEPARATOR_SIZE) / zoomScale / 2.f, CELL_SIZE_Y / zoomScale / 2.f);
			pos.x += (cur_bucket + 1) * (CELL_SIZE_X + HT_SEPARATOR_SIZE) / zoomScale;

			followNode(ImVec2(pos.x, pos.y - viewport->WorkSize.y / 3.f / zoomScale));

			if (iteratingNode != nullptr) {
				followNode(ImVec2(iteratingNode->curPos.x, iteratingNode->curPos.y - viewport->WorkSize.y / 3.f / zoomScale));
				if (iteratingNode->value == searchVal) {
					found = true;
					iteratingNode->color = FAIL_NODE_COL;
				}
				else
					iteratingNode->color = ITER_NODE_COL;
			}

		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Find")) {
		if (add_node_text[0]) {
			cur_bucket = hashValue(add_node_text);
			searchVal = add_node_text;
			memset(add_node_text, 0, sizeof add_node_text);
			mode = 2;

			iteratingNode = buckets[cur_bucket];

			ImVec2 pos(-((table_size + 1) * CELL_SIZE_X + (table_size + 2) * HT_SEPARATOR_SIZE) / zoomScale / 2.f, CELL_SIZE_Y / zoomScale / 2.f);
			pos.x += (cur_bucket + 1) * (CELL_SIZE_X + HT_SEPARATOR_SIZE) / zoomScale;

			followNode(ImVec2(pos.x, pos.y - viewport->WorkSize.y / 3.f / zoomScale));

			if (iteratingNode != nullptr) {
				followNode(ImVec2(iteratingNode->curPos.x, iteratingNode->curPos.y + -viewport->WorkSize.y / 3.f / zoomScale));
				if (iteratingNode->value == searchVal) {
					found = true;
					iteratingNode->color = FOUND_NODE_COL;
				}
				else
					iteratingNode->color = ITER_NODE_COL;
			}

		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Erase")) {
		if (add_node_text[0]) {
			cur_bucket = hashValue(add_node_text);
			searchVal = add_node_text;
			memset(add_node_text, 0, sizeof add_node_text);
			mode = 3;

			ImVec2 pos(-((table_size + 1) * CELL_SIZE_X + (table_size + 2) * HT_SEPARATOR_SIZE) / zoomScale / 2.f, CELL_SIZE_Y / zoomScale / 2.f);
			pos.x += (cur_bucket + 1) * (CELL_SIZE_X + HT_SEPARATOR_SIZE) / zoomScale;

			followNode(ImVec2(pos.x, pos.y - viewport->WorkSize.y / 3.f / zoomScale));
		}
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

	ImVec2 s_pos(center.x + camPos.x * zoomScale - (CELL_SIZE_X + HT_SEPARATOR_SIZE) * table_size / 2.f, center.y + camPos.y * zoomScale - CELL_SIZE_Y - viewport->WorkSize.y / 3.f);
	ImVec2 cur_pos(s_pos);

	ImVec2 targetPos(-((table_size + 1) * CELL_SIZE_X + (table_size + 2) * HT_SEPARATOR_SIZE) / zoomScale / 2.f, CELL_SIZE_Y / zoomScale / 2.f);

	for (int i = 0; i < table_size; i++) {

		targetPos.x += (CELL_SIZE_X + HT_SEPARATOR_SIZE) / zoomScale;
		targetPos.y = -CELL_SIZE_Y / zoomScale / 2.f;

		if (tempNode != nullptr) {
			if(tempNode->next != nullptr)
			drawEdge(ImVec2(tempNode->curPos.x, tempNode->curPos.y - viewport->WorkSize.y / 3.f / zoomScale), ImVec2(tempNode->next->curPos.x, tempNode->next->curPos.y - viewport->WorkSize.y / 3.f / zoomScale));

			ImVec2 textCenter = ImGui::CalcTextSize(tempNode->value.c_str());
			textCenter.x /= 2.f;
			textCenter.y /= 2.f;

			draw_list->AddCircleFilled(ImVec2(center.x + (camPos.x + tempNode->curPos.x) * zoomScale, center.y + (camPos.y + tempNode->curPos.y) * zoomScale - viewport->WorkSize.y / 3.f), VERTEX_RADIUS, tempNode->color);
			draw_list->AddText(ImVec2(center.x + (camPos.x + tempNode->curPos.x) * zoomScale - textCenter.x, center.y + (camPos.y + tempNode->curPos.y) * zoomScale - textCenter.y - viewport->WorkSize.y / 3.f), TEXT_COL, tempNode->value.c_str());
		}

		ImVec2 prevPos(targetPos);

		Node* cur_node = buckets[i];

		while (cur_node != nullptr) {
			drawEdge(ImVec2(prevPos. x, prevPos.y - viewport->WorkSize.y / 3.f / zoomScale), ImVec2(cur_node->curPos.x, cur_node->curPos.y - viewport->WorkSize.y / 3.f / zoomScale));
			if(cur_node->value == toDelete)
				targetPos.x += NODES_DIST / 2.f / zoomScale;

			targetPos.y += NODES_DIST / zoomScale;

			ImVec2 textCenter = ImGui::CalcTextSize(cur_node->value.c_str());
			textCenter.x /= 2.f;
			textCenter.y /= 2.f;

			draw_list->AddCircleFilled(ImVec2(center.x + (camPos.x + cur_node->curPos.x) * zoomScale, center.y + (camPos.y + cur_node->curPos.y) * zoomScale - viewport->WorkSize.y / 3.f), VERTEX_RADIUS, cur_node->color);

			draw_list->AddText(ImVec2(center.x + (camPos.x + cur_node->curPos.x) * zoomScale - textCenter.x, center.y + (camPos.y + cur_node->curPos.y) * zoomScale - textCenter.y - viewport->WorkSize.y / 3.f), TEXT_COL, cur_node->value.c_str());

			cur_node->curPos.x += (targetPos.x - cur_node->curPos.x) * 10.f * io->DeltaTime;
			cur_node->curPos.y += (targetPos.y - cur_node->curPos.y) * 10.f * io->DeltaTime;

			if (cur_node->value == toDelete) {
				targetPos.x -= NODES_DIST / 2.f / zoomScale;
				targetPos.y -= NODES_DIST / zoomScale;
			}

			prevPos = cur_node->curPos;
			cur_node = cur_node->next;
		}

		std::string bucket_label = std::to_string(i);
		ImVec2 textSize = ImGui::CalcTextSize(bucket_label.c_str());
		ImVec2 bucketPos((cur_pos.x + (CELL_SIZE_X - textSize.x) / 2.f), (cur_pos.y + (CELL_SIZE_Y - textSize.y) / 2.f));
		ImU32 col = IM_COL32(255, 255, 255, 255);
		draw_list->AddRectFilled(cur_pos, ImVec2(cur_pos.x + std::max(CELL_SIZE_X, textSize.x), cur_pos.y + CELL_SIZE_Y), (i == cur_bucket ? CUR_BUCKET_COL : DEFAULT_BUCKET_COL));
		draw_list->AddText(bucketPos, col, bucket_label.c_str());
		cur_pos.x += std::max(CELL_SIZE_X, textSize.x) + HT_SEPARATOR_SIZE;
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

void HashTable::followNode(ImVec2 pos)
{
	if (!camFollow)
		return;
	camTarget = ImVec2(-pos.x, -pos.y);
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

HashTable::HashTable(std::string name, int& state, float& scale, bool& settingEnabled)
	: GrandWindow(name, state, scale, settingsEnabled)
{
	buckets = std::vector<Node*>(table_size, nullptr);
}

HashTable::~HashTable()
{

}

void HashTable::update()
{
	center = ImVec2(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);

	ImGui::Begin(getName().c_str(), NULL, main_flags);

	ImGui::PopStyleVar();

	tableUpdate();


	if (mode != 0) {

		curTime += io->DeltaTime;

		while (curTime * speed >= HT_DELAY) {
			curTime -= HT_DELAY / speed;

			if (mode == 1) {

				if (buckets[cur_bucket] == nullptr) {
					ImVec2 pos(-((table_size + 1) * CELL_SIZE_X + (table_size + 2) * HT_SEPARATOR_SIZE) / zoomScale / 2.f, CELL_SIZE_Y / zoomScale / 2.f);
					pos.x += (cur_bucket + 1) * (CELL_SIZE_X + HT_SEPARATOR_SIZE) / zoomScale;
					buckets[cur_bucket] = new Node(searchVal, pos);
					followNode(ImVec2(buckets[cur_bucket]->curPos.x, buckets[cur_bucket]->curPos.y + NODES_DIST / zoomScale - viewport->WorkSize.y / 3.f / zoomScale));
					cur_bucket = -1;
					mode = 0;
				}
				else {
					if (iteratingNode->next != nullptr && !found) {
						iteratingNode->color = DEFAULT_NODE_COL;
						iteratingNode = iteratingNode->next;
						followNode(ImVec2(iteratingNode->curPos.x, iteratingNode->curPos.y + -viewport->WorkSize.y / 3.f / zoomScale));
						if (iteratingNode->value == searchVal) {
							found = true;
							iteratingNode->color = FAIL_NODE_COL;
						}
						else
							iteratingNode->color = ITER_NODE_COL;
					}
					else {
						iteratingNode->color = DEFAULT_NODE_COL;
						if (!found){
							iteratingNode->next = new Node(searchVal, iteratingNode->curPos);
							followNode(ImVec2(iteratingNode->curPos.x, iteratingNode->curPos.y + NODES_DIST / zoomScale - viewport->WorkSize.y / 3.f / zoomScale));
						}
						iteratingNode = nullptr;
						found = false;
						cur_bucket = -1;
						mode = 0;
					}
				}
			}
			else if (mode == 2) {
				if (iteratingNode != nullptr && !found) {
					iteratingNode->color = DEFAULT_NODE_COL;
					iteratingNode = iteratingNode->next;

					if (iteratingNode != nullptr) {
						followNode(ImVec2(iteratingNode->curPos.x, iteratingNode->curPos.y - viewport->WorkSize.y / 3.f / zoomScale));
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
						followNode(ImVec2(iteratingNode->curPos.x, iteratingNode->curPos.y - viewport->WorkSize.y / 3.f / zoomScale));
						iteratingNode->color = DEFAULT_NODE_COL;
						iteratingNode = nullptr;
					}
					else {
						found = false;
						cur_bucket = -1;
						mode = 0;
					}
				}
			}
			else if (mode == 3) {

				if (tempNode != nullptr) {
					if (iteratingNode != nullptr) {
						iteratingNode->color = DEFAULT_NODE_COL;
						iteratingNode = nullptr;
					}
					toDelete.clear();
					delete tempNode;
					tempNode = nullptr;
					cur_bucket = -1;
					mode = 0;
				}
				else if (iteratingNode == nullptr) {

					if (buckets[cur_bucket] == nullptr) {
						cur_bucket = -1;
						mode = 0;
					}
					else if (buckets[cur_bucket]->value == searchVal) {
						if (toDelete.empty()) {
							toDelete = buckets[cur_bucket]->value;
							followNode(ImVec2(buckets[cur_bucket]->curPos.x, buckets[cur_bucket]->curPos.y - viewport->WorkSize.y / 3.f / zoomScale));
						}
						else {
							tempNode = buckets[cur_bucket];
							tempNode->color = FAIL_NODE_COL;
							buckets[cur_bucket] = buckets[cur_bucket]->next;
						}
					}
					else {
						iteratingNode = buckets[cur_bucket];
						followNode(ImVec2(iteratingNode->curPos.x, iteratingNode->curPos.y - viewport->WorkSize.y / 3.f / zoomScale));
						iteratingNode->color = ITER_NODE_COL;
					}
				}
				else {

					if (iteratingNode->next != nullptr && iteratingNode->next->value != searchVal) {
						iteratingNode->color = DEFAULT_NODE_COL;
						iteratingNode = iteratingNode->next;
						followNode(ImVec2(iteratingNode->curPos.x, iteratingNode->curPos.y - viewport->WorkSize.y / 3.f / zoomScale));
						iteratingNode->color = ITER_NODE_COL;
					}
					else if (iteratingNode->next != nullptr) {
						if (toDelete.empty()) {
							toDelete = iteratingNode->next->value;
							followNode(ImVec2(iteratingNode->next->curPos.x, iteratingNode->next->curPos.y - viewport->WorkSize.y / 3.f / zoomScale));
						}
						else {
							tempNode = iteratingNode->next;
							tempNode->color = FAIL_NODE_COL;
							iteratingNode->next = tempNode->next;
						}
					}
					else {
						iteratingNode->color = DEFAULT_NODE_COL;
						iteratingNode = nullptr;
						cur_bucket = -1;
						mode = 0;
					}

				}

			}
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

	camPos.x += (camTarget.x - camPos.x) * 10.f * io->DeltaTime;
	camPos.y += (camTarget.y - camPos.y) * 10.f * io->DeltaTime;

	if (ImGui::IsWindowHovered() && io->MouseWheel != 0.0f) {
		zoomScale += io->MouseWheel * 0.15f;
		zoomScale = std::min(std::max(zoomScale, 0.5f), 3.0f);
	}

	ImGui::End();

	controlsUpdate();

}
