#include "HashMap.h"
#include <imgui_internal.h>
#include <iostream>

void HashMap::controlsUpdate()
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

	ImGui::InputText("Key", key_text, IM_ARRAYSIZE(key_text));
	ImGui::InputText("Value", value_text, IM_ARRAYSIZE(value_text));


	if (!key_text[0] && !disabled) {
		disabled = true;
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	bool noVal = false;
	if (!value_text[0] && !disabled) {
		noVal = true;
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	if (ImGui::Button("Insert")) {
		if (key_text[0] && value_text[0]) {
			cur_bucket = hashValue(key_text);
			searchKey = key_text;
			newVal = value_text;
			memset(key_text, 0, sizeof key_text);
			memset(value_text, 0, sizeof value_text);
			mode = 1;

			iteratingNode = buckets[cur_bucket];

			ImVec2 pos(-((table_size + 1) * CELL_SIZE_X + (table_size + 2) * HM_SEPARATOR_SIZE) / zoomScale / 2.f, CELL_SIZE_Y / zoomScale / 2.f);
			pos.x += (cur_bucket + 1) * (CELL_SIZE_X + HM_SEPARATOR_SIZE) / zoomScale;

			followNode(ImVec2(pos.x, pos.y - viewport->WorkSize.y / 3.f / zoomScale));

			if (iteratingNode != nullptr) {
				followNode(ImVec2(iteratingNode->curPos.x, iteratingNode->curPos.y - viewport->WorkSize.y / 3.f / zoomScale));
				if (iteratingNode->key == searchKey) {
					found = true;
					iteratingNode->keyColor = FOUND_NODE_COL;
				}
				else
					iteratingNode->keyColor = ITER_NODE_COL;
			}

		}
	}

	if (noVal) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	ImGui::SameLine();

	if (ImGui::Button("Find")) {
		if (key_text[0]) {
			cur_bucket = hashValue(key_text);
			searchKey = key_text;
			memset(key_text, 0, sizeof key_text);
			mode = 2;

			iteratingNode = buckets[cur_bucket];

			ImVec2 pos(-((table_size + 1) * CELL_SIZE_X + (table_size + 2) * HM_SEPARATOR_SIZE) / zoomScale / 2.f, CELL_SIZE_Y / zoomScale / 2.f);
			pos.x += (cur_bucket + 1) * (CELL_SIZE_X + HM_SEPARATOR_SIZE) / zoomScale;

			followNode(ImVec2(pos.x, pos.y - viewport->WorkSize.y / 3.f / zoomScale));

			if (iteratingNode != nullptr) {
				followNode(ImVec2(iteratingNode->curPos.x, iteratingNode->curPos.y + -viewport->WorkSize.y / 3.f / zoomScale));
				if (iteratingNode->key == searchKey) {
					found = true;
					iteratingNode->keyColor = FOUND_NODE_COL;
				}
				else
					iteratingNode->keyColor = ITER_NODE_COL;
			}

		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Erase")) {
		if (key_text[0]) {
			cur_bucket = hashValue(key_text);
			searchKey = key_text;
			memset(key_text, 0, sizeof key_text);
			mode = 3;

			ImVec2 pos(-((table_size + 1) * CELL_SIZE_X + (table_size + 2) * HM_SEPARATOR_SIZE) / zoomScale / 2.f, CELL_SIZE_Y / zoomScale / 2.f);
			pos.x += (cur_bucket + 1) * (CELL_SIZE_X + HM_SEPARATOR_SIZE) / zoomScale;

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

	ImGui::SliderFloat("Speed", &speed, HM_MIN_SPEED, HM_MAX_SPEED, "%.1fx", ImGuiSliderFlags_AlwaysClamp);

	ImGui::End();

}

void HashMap::tableUpdate()
{

	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	ImVec2 s_pos(center.x + camPos.x * zoomScale - (CELL_SIZE_X + HM_SEPARATOR_SIZE) * table_size / 2.f, center.y + camPos.y * zoomScale - CELL_SIZE_Y - viewport->WorkSize.y / 3.f);
	ImVec2 cur_pos(s_pos);

	ImVec2 targetPos(-((table_size + 1) * CELL_SIZE_X + (table_size + 2) * HM_SEPARATOR_SIZE) / zoomScale / 2.f, CELL_SIZE_Y / zoomScale / 2.f);

	for (int i = 0; i < table_size; i++) {

		targetPos.x += (CELL_SIZE_X + HM_SEPARATOR_SIZE) / zoomScale;
		targetPos.y = -CELL_SIZE_Y / zoomScale / 2.f;

		if (tempNode != nullptr) {
			if (tempNode->next != nullptr)
				drawEdge(ImVec2(tempNode->curPos.x - LINK_DIST / 2.f, tempNode->curPos.y - viewport->WorkSize.y / 3.f / zoomScale), ImVec2(tempNode->next->curPos.x - LINK_DIST / 2.f, tempNode->next->curPos.y - viewport->WorkSize.y / 3.f / zoomScale));

			drawEdge(ImVec2(tempNode->curPos.x - LINK_DIST / 2.f, tempNode->curPos.y - viewport->WorkSize.y / 3.f / zoomScale), ImVec2(tempNode->curPos.x + LINK_DIST / 2.f, tempNode->curPos.y - viewport->WorkSize.y / 3.f / zoomScale));

			ImVec2 textCenter = ImGui::CalcTextSize(tempNode->key.c_str());
			textCenter.x /= 2.f;
			textCenter.y /= 2.f;

			draw_list->AddCircleFilled(ImVec2(center.x + (camPos.x + tempNode->curPos.x) * zoomScale - LINK_DIST * zoomScale / 2.f, center.y + (camPos.y + tempNode->curPos.y) * zoomScale - viewport->WorkSize.y / 3.f), VERTEX_RADIUS, tempNode->keyColor);
			draw_list->AddText(ImVec2(center.x + (camPos.x + tempNode->curPos.x) * zoomScale - textCenter.x - LINK_DIST * zoomScale / 2.f, center.y + (camPos.y + tempNode->curPos.y) * zoomScale - textCenter.y - viewport->WorkSize.y / 3.f), TEXT_COL, tempNode->key.c_str());

			textCenter = ImGui::CalcTextSize(tempNode->value.c_str());
			textCenter.x /= 2.f;
			textCenter.y /= 2.f;

			draw_list->AddCircleFilled(ImVec2(center.x + (camPos.x + tempNode->curPos.x) * zoomScale + LINK_DIST * zoomScale / 2.f, center.y + (camPos.y + tempNode->curPos.y) * zoomScale - viewport->WorkSize.y / 3.f), VERTEX_RADIUS, tempNode->valColor);
			draw_list->AddText(ImVec2(center.x + (camPos.x + tempNode->curPos.x) * zoomScale - textCenter.x + LINK_DIST * zoomScale / 2.f, center.y + (camPos.y + tempNode->curPos.y) * zoomScale - textCenter.y - viewport->WorkSize.y / 3.f), TEXT_COL, tempNode->value.c_str());
		}

		ImVec2 prevPos(targetPos);

		Node* cur_node = buckets[i];

		while (cur_node != nullptr) {
			drawEdge(ImVec2(prevPos.x - LINK_DIST / 2.f, prevPos.y - viewport->WorkSize.y / 3.f / zoomScale), ImVec2(cur_node->curPos.x - LINK_DIST / 2.f, cur_node->curPos.y - viewport->WorkSize.y / 3.f / zoomScale));
			
			drawEdge(ImVec2(cur_node->curPos.x - LINK_DIST / 2.f, cur_node->curPos.y - viewport->WorkSize.y / 3.f / zoomScale), ImVec2(cur_node->curPos.x + LINK_DIST / 2.f, cur_node->curPos.y - viewport->WorkSize.y / 3.f / zoomScale));
			
			if (cur_node->key == toDelete) {
				targetPos.x += NODES_DIST / 2.f / zoomScale;
				targetPos.y += NODES_DIST / zoomScale / 2.f;
			}
			else
				targetPos.y += NODES_DIST / zoomScale;

			ImVec2 textCenter = ImGui::CalcTextSize(cur_node->key.c_str());
			textCenter.x /= 2.f;
			textCenter.y /= 2.f;

			draw_list->AddCircleFilled(ImVec2(center.x + (camPos.x + cur_node->curPos.x) * zoomScale - LINK_DIST * zoomScale / 2.f, center.y + (camPos.y + cur_node->curPos.y) * zoomScale - viewport->WorkSize.y / 3.f), VERTEX_RADIUS, cur_node->keyColor);
			draw_list->AddText(ImVec2(center.x + (camPos.x + cur_node->curPos.x) * zoomScale - textCenter.x - LINK_DIST * zoomScale / 2.f, center.y + (camPos.y + cur_node->curPos.y) * zoomScale - textCenter.y - viewport->WorkSize.y / 3.f), TEXT_COL, cur_node->key.c_str());

			textCenter = ImGui::CalcTextSize(cur_node->value.c_str());
			textCenter.x /= 2.f;
			textCenter.y /= 2.f;

			draw_list->AddCircleFilled(ImVec2(center.x + (camPos.x + cur_node->curPos.x) * zoomScale + LINK_DIST * zoomScale / 2.f, center.y + (camPos.y + cur_node->curPos.y) * zoomScale - viewport->WorkSize.y / 3.f), VERTEX_RADIUS, cur_node->valColor);
			draw_list->AddText(ImVec2(center.x + (camPos.x + cur_node->curPos.x) * zoomScale - textCenter.x + LINK_DIST * zoomScale / 2.f, center.y + (camPos.y + cur_node->curPos.y) * zoomScale - textCenter.y - viewport->WorkSize.y / 3.f), TEXT_COL, cur_node->value.c_str());

			cur_node->curPos.x += (targetPos.x - cur_node->curPos.x) * 10.f * io->DeltaTime;
			cur_node->curPos.y += (targetPos.y - cur_node->curPos.y) * 10.f * io->DeltaTime;

			if (cur_node->key == toDelete) {
				targetPos.x -= NODES_DIST / 2.f / zoomScale;
				targetPos.y -= NODES_DIST / zoomScale / 2.f;
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
		cur_pos.x += std::max(CELL_SIZE_X, textSize.x) + HM_SEPARATOR_SIZE;
	}

}

float HashMap::calcDist(float x1, float y1, float x2, float y2)
{
	return sqrtf((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void HashMap::drawEdge(ImVec2 u, ImVec2 v) {

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

void HashMap::followNode(ImVec2 pos)
{
	if (!camFollow)
		return;
	camTarget = ImVec2(-pos.x, -pos.y);
}

int HashMap::hashValue(std::string value)
{
	int hash = 0;

	for (int i = 0; i < value.size(); i++) {
		hash = (hash * 257) % table_size;
		hash = (hash + value[i]) % table_size;
	}

	return hash;
}

HashMap::HashMap(std::string name, int& state, float& scale, bool& settingEnabled)
	: GrandWindow(name, state, scale, settingsEnabled)
{
	buckets = std::vector<Node*>(table_size, nullptr);
}

HashMap::~HashMap()
{

}

void HashMap::update()
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

		while (curTime * speed >= HM_DELAY) {
			curTime -= HM_DELAY / speed;

			if (mode == 1) {

				if (buckets[cur_bucket] == nullptr) {
					ImVec2 pos(-((table_size + 1) * CELL_SIZE_X + (table_size + 2) * HM_SEPARATOR_SIZE) / zoomScale / 2.f, CELL_SIZE_Y / zoomScale / 2.f);
					pos.x += (cur_bucket + 1) * (CELL_SIZE_X + HM_SEPARATOR_SIZE) / zoomScale;
					buckets[cur_bucket] = new Node(searchKey, newVal, pos);
					followNode(ImVec2(buckets[cur_bucket]->curPos.x, buckets[cur_bucket]->curPos.y + NODES_DIST / zoomScale - viewport->WorkSize.y / 3.f / zoomScale));
					cur_bucket = -1;
					mode = 0;
				}
				else {
					if (iteratingNode->next != nullptr && !found) {
						iteratingNode->keyColor = DEFAULT_NODE_COL;
						iteratingNode = iteratingNode->next;
						followNode(ImVec2(iteratingNode->curPos.x, iteratingNode->curPos.y + -viewport->WorkSize.y / 3.f / zoomScale));
						if (iteratingNode->key == searchKey) {
							found = true;
							iteratingNode->keyColor = FOUND_NODE_COL;
						}
						else
							iteratingNode->keyColor = ITER_NODE_COL;
					}
					else {
						iteratingNode->keyColor = DEFAULT_NODE_COL;
						if (!found) {
							iteratingNode->next = new Node(searchKey, newVal, iteratingNode->curPos);
							followNode(ImVec2(iteratingNode->curPos.x, iteratingNode->curPos.y + NODES_DIST / zoomScale - viewport->WorkSize.y / 3.f / zoomScale));
							iteratingNode = nullptr;
							found = false;
							cur_bucket = -1;
							mode = 0;
						}
						else {
							if (iteratingNode->valColor != FOUND_NODE_COL) {
								iteratingNode->valColor = FOUND_NODE_COL;
								iteratingNode->value = newVal;
							}
							else {
								iteratingNode->valColor = DEFAULT_NODE_COL;
								iteratingNode = nullptr;
								found = false;
								cur_bucket = -1;
								mode = 0;
							}
						}
					}
				}
			}
			else if (mode == 2) {
				if (iteratingNode != nullptr && !found) {
					iteratingNode->keyColor = DEFAULT_NODE_COL;
					iteratingNode = iteratingNode->next;

					if (iteratingNode != nullptr) {
						followNode(ImVec2(iteratingNode->curPos.x, iteratingNode->curPos.y - viewport->WorkSize.y / 3.f / zoomScale));
						if (iteratingNode->key == searchKey) {
							found = true;
							iteratingNode->keyColor = FOUND_NODE_COL;
						}
						else
							iteratingNode->keyColor = ITER_NODE_COL;
					}
				}
				else {
					if (iteratingNode != nullptr) {
						if (iteratingNode->keyColor != DEFAULT_NODE_COL) {
							followNode(ImVec2(iteratingNode->curPos.x, iteratingNode->curPos.y - viewport->WorkSize.y / 3.f / zoomScale));
							iteratingNode->keyColor = DEFAULT_NODE_COL;
							iteratingNode->valColor = FOUND_NODE_COL;
						}
						else {
							iteratingNode->valColor = DEFAULT_NODE_COL;
							iteratingNode = nullptr;
						}
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
						iteratingNode->keyColor = DEFAULT_NODE_COL;
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
					else if (buckets[cur_bucket]->key == searchKey) {
						if (toDelete.empty()) {
							toDelete = buckets[cur_bucket]->key;
							followNode(ImVec2(buckets[cur_bucket]->curPos.x, buckets[cur_bucket]->curPos.y - viewport->WorkSize.y / 3.f / zoomScale));
						}
						else {
							tempNode = buckets[cur_bucket];
							tempNode->keyColor = FAIL_NODE_COL;
							buckets[cur_bucket] = buckets[cur_bucket]->next;
						}
					}
					else {
						iteratingNode = buckets[cur_bucket];
						followNode(ImVec2(iteratingNode->curPos.x, iteratingNode->curPos.y - viewport->WorkSize.y / 3.f / zoomScale));
						iteratingNode->keyColor = ITER_NODE_COL;
					}
				}
				else {

					if (iteratingNode->next != nullptr && iteratingNode->next->key != searchKey) {
						iteratingNode->keyColor = DEFAULT_NODE_COL;
						iteratingNode = iteratingNode->next;
						followNode(ImVec2(iteratingNode->curPos.x, iteratingNode->curPos.y - viewport->WorkSize.y / 3.f / zoomScale));
						iteratingNode->keyColor = ITER_NODE_COL;
					}
					else if (iteratingNode->next != nullptr) {
						if (toDelete.empty()) {
							toDelete = iteratingNode->next->key;
							followNode(ImVec2(iteratingNode->next->curPos.x, iteratingNode->next->curPos.y - viewport->WorkSize.y / 3.f / zoomScale));
						}
						else {
							tempNode = iteratingNode->next;
							tempNode->keyColor = FAIL_NODE_COL;
							iteratingNode->next = tempNode->next;
						}
					}
					else {
						iteratingNode->keyColor = DEFAULT_NODE_COL;
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
