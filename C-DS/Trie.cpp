#include "Trie.h"
#include <imgui_internal.h>
#include <iostream>

void Trie::controlsUpdate()
{
	ImVec2 controlsWinSize(std::min(450.f * GuiScale, viewport->WorkSize.x - ImGui::GetStyle().WindowPadding.x), std::min(750.f * GuiScale, viewport->WorkSize.y - 2 * ImGui::GetStyle().WindowPadding.y));
	ImVec2 controlsWinPos(viewport->Size.x - controlsWinSize.x - ImGui::GetStyle().WindowPadding.x, viewport->Size.y - controlsWinSize.y - ImGui::GetStyle().WindowPadding.y);

	ImGui::SetNextWindowSize(controlsWinSize);
	ImGui::SetNextWindowPos(controlsWinPos);

	ImGui::Begin("Controls", NULL, controls_flags);

	if (ImGui::Button("Back"))
		state = 0;

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	ImGui::Text("Tools:");
	ImGui::RadioButton("Move Camera", &cur_tool, 1);
	if (camTarget.x != 0 || camTarget.y != 0) {
		ImGui::SameLine();
		if (ImGui::Button("Reset##Move_Cam"))
			camTarget = { 0, 0 };
	}

	if (disableButtons) {
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	ImGui::InputText("##Insert", add_node_text[0], IM_ARRAYSIZE(add_node_text[0]));

	ImGui::SameLine();

	if (ImGui::Button("Insert Word")) {
		char* ptr = add_node_text[0];
		while (*ptr != '\0') {
			word.push_back(*ptr);
			ptr++;
		}
		if (word.size()) {
			disableButtons = 1;
			trueUpdate = 1;
			curNode = 0, curIdx = 0;
			memset(add_node_text, 0, sizeof add_node_text);
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}

	}

	ImGui::InputText("##FideW", add_node_text[1], IM_ARRAYSIZE(add_node_text[1]));

	ImGui::SameLine();

	if (ImGui::Button("Find Word")) {
		std::string curNode;
		char* ptr = add_node_text[1];
		while (*ptr != '\0') {
			curNode.push_back(*ptr);
			ptr++;
		}
		if (curNode.size()) {
			disableButtons = 1;
			trueUpdate = search(curNode, false);
			curNode.clear();
			memset(add_node_text, 0, sizeof add_node_text);
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}

	}

	ImGui::InputText("##FindP", add_node_text[2], IM_ARRAYSIZE(add_node_text[2]));

	ImGui::SameLine();

	if (ImGui::Button("Find Prefix")) {
		std::string curNode;
		char* ptr = add_node_text[2];
		while (*ptr != '\0') {
			curNode.push_back(*ptr);
			ptr++;
		}
		if (curNode.size()) {
			disableButtons = 1;
			trueUpdate = search(curNode, true);
			curNode.clear();
			memset(add_node_text, 0, sizeof add_node_text);
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}

	}

	ImGui::InputText("##Delete", add_node_text[3], IM_ARRAYSIZE(add_node_text[3]));

	ImGui::SameLine();

	if (ImGui::Button("Delete Word")) {
		std::string curNode;
		char* ptr = add_node_text[3];
		while (*ptr != '\0') {
			curNode.push_back(*ptr);
			ptr++;
		}
		if (curNode.size()) {
			disableButtons = 1;
			deleting = 1;
			trueUpdate = search(curNode, false);
			endw = updatedNodes.back();
			curNode.clear();
			memset(add_node_text, 0, sizeof add_node_text);
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}

	}

	if (ImGui::Button("Clear")) {
		clear();
		memset(add_node_text, 0, sizeof add_node_text);
	}

    if (disableButtons) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	ImGui::SliderFloat("Speed", &speed, TRIE_MIN_SPEED, TRIE_MAX_SPEED, "%.1fx", ImGuiSliderFlags_AlwaysClamp);

	ImGui::End();
}

ImU32 Trie::getColor(int color_code)
{
	switch (color_code) {
	case DEF_VERT_COL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(200, 200, 200, 255)) : ImGui::GetColorU32(IM_COL32(150, 150, 150, 255));
	case MARKED_VERT_COL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(50, 200, 200, 255)) : ImGui::GetColorU32(IM_COL32(50, 150, 150, 255));
	case INCORRECT:
		return colorMode ? ImGui::GetColorU32(IM_COL32(200, 50, 50, 255)) : ImGui::GetColorU32(IM_COL32(150, 50, 50, 255));
	case VISITED_VERT_COL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(50, 200, 50, 255)) : ImGui::GetColorU32(IM_COL32(50, 150, 50, 255));
	case UPDATED_NODE_COLOR:
		return colorMode ? ImGui::GetColorU32(IM_COL32(50, 200, 50, 255)) : ImGui::GetColorU32(IM_COL32(50, 150, 50, 255));
	case VERT_BORDER_COL:
		return ImGui::GetColorU32(IM_COL32(40, 40, 40, 255));
	case FIXED_VERT_COLOR:
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

void Trie::drawText(ImVec2 pos, const char* text)
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

float Trie::calcDist(float x1, float y1, float x2, float y2)
{
	return sqrtf((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void Trie::graphUpdate()
{
	const ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	const float cf = 0.55f; // center attraction
	const float k = 5.f; // Spring constant
	const float c = 8000.f; // Repulsion constant
	const float dampening = 0.85f; // Dampening factor
	for (int i = 0; i != nodes.size(); i++) {
		if (nodes[i].cnt == 0)continue;
		auto& u = nodes[i];
		u.fx -= cf * u.x;
		u.fy -= cf * u.y;
		for (int j = 0; j != nodes.size(); j++) {
			if (nodes[j].cnt == 0)continue;
			if (i != j) {
				auto& v = nodes[j];

				float d = calcDist(u.x, u.y, v.x, v.y);
				float dx = v.x - u.x;
				float dy = v.y - u.y;
				float force = c / (d * d);

				u.fx -= force * dx;
				u.fy -= force * dy;
			}
		}
	}

	for (int j = 0; j != nodes.size(); j++) {
		if (nodes[j].cnt == 0)continue;
		if (j != parent[j]) {
			auto& u = nodes[j];
			auto& v = nodes[parent[j]];

			float dx = v.x - u.x;
			float dy = v.y - u.y;
			float d = calcDist(u.x, u.y, v.x, v.y);

			float force = k * (d - TRIE_EDGE_LENGTH);

			u.fx += force * dx / d;
			u.fy += force * dy / d;
			v.fx -= force * dx / d;
			v.fy -= force * dy / d;
		}
	}

	for (int j = 0; j != nodes.size(); j++) {
		if (nodes[j].cnt == 0)continue;
		auto& node = nodes[j];
		ImVec2 pos;
		pos.x = center.x + (camPos.x + node.x) * zoomScale;
		pos.y = center.y + (camPos.y + node.y) * zoomScale;
		// -_- 
		float dist = calcDist(pos.x, pos.y, io->MousePos.x, io->MousePos.y);

		if (ImGui::IsMouseDown(0) && dragging == -1 && dist <= TRIE_VERTEX_RADIUS * zoomScale && ImGui::IsWindowHovered() && cur_tool == 0) {
			dragging = j;
		}
		else if ((!ImGui::IsWindowFocused() || !ImGui::IsMouseDown(0 || cur_tool != 0)) && dragging != -1) {
			dragging = -1;
		}
		/*
		if (ImGui::IsMouseDoubleClicked(0) && dist <= TRIE_VERTEX_RADIUS * zoomScale && ImGui::IsWindowHovered() && cur_tool == 0) {
			node.second.fixed = !node.second.fixed;
		}
		*/
		/*
		if (node.second.searching) {
			node.second.color = VISITED_VERT_COL;
		}
		else if (sameGroup(node.first, viewComponent)) {
			node.second.color = MARKED_VERT_COL;
		}
		else
			node.second.color = DEF_VERT_COL;
			*/

		if (dragging == j) {
			node.fx = node.fy = 0;
			node.x += io->MouseDelta.x / zoomScale;
			node.y += io->MouseDelta.y / zoomScale;
			continue;
		}

		if (node.fixed) {
			node.fx = node.fy = 0;
			node.y = -center.y * 0.8f;
			node.x = 0;
			continue;
		}
		node.x += node.fx * io->DeltaTime;
		node.y += node.fy * io->DeltaTime;
		node.fx *= dampening;
		node.fy *= dampening;
	}

	if (ImGui::IsMouseDown(0) && dragging == -1 && ImGui::IsWindowFocused() && cur_tool == 0) {
		dragging = -2;
	}

	for (int j = 1; j != parent.size(); j++) {
		if (nodes[j].cnt == 0)continue;
		drawEdge(draw_list, j, parent[j]);
	}

	for (auto& node : nodes) {
		if (node.cnt == 0)continue;
		ImVec2 textCenter = ImGui::CalcTextSize(node.val.c_str());
		textCenter.x /= 2.f;
		textCenter.y /= 2.f;

		draw_list->AddCircleFilled(ImVec2(center.x + (camPos.x + node.x) * zoomScale, center.y + (camPos.y + node.y) * zoomScale), TRIE_VERTEX_RADIUS* zoomScale, getColor(node.color));
		draw_list->AddCircle(ImVec2(center.x + (camPos.x + node.x) * zoomScale, center.y + (camPos.y + node.y) * zoomScale), TRIE_VERTEX_RADIUS* zoomScale, getColor(VERT_BORDER_COL), 100, 5.f * zoomScale);

		if (node.fixed)
			draw_list->AddCircle(ImVec2(center.x + (camPos.x + node.x) * zoomScale, center.y + (camPos.y + node.y) * zoomScale), TRIE_VERTEX_RADIUS * zoomScale, getColor(FIXED_VERT_COLOR), 100, 10.f * zoomScale);
		drawText(ImVec2(center.x + (camPos.x + node.x) * zoomScale - textCenter.x, center.y + (camPos.y + node.y) * zoomScale - textCenter.y), node.val.c_str());
	}

	if (ImGui::IsMouseDown(0))
		leftClickPressed = true;
	else
		leftClickPressed = false;

}

void Trie::drawEdge(ImDrawList* draw_list, int u, int v)
{
	ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);

	ImVec2 from = ImVec2(center.x + (camPos.x + nodes[v].x) * zoomScale, center.y + (camPos.y + nodes[v].y) * zoomScale);
	ImVec2 to = ImVec2(center.x + (camPos.x + nodes[u].x) * zoomScale, center.y + (camPos.y + nodes[u].y) * zoomScale);

	const float thickness = 5.f * zoomScale;

	ImVec2 dir = ImVec2(to.x - from.x, to.y - from.y);
	float length = calcDist(from.x, from.y, to.x, to.y);
	dir.x /= length;
	dir.y /= length;

	const float headSize = 15.f * zoomScale;
	to.x -= dir.x * TRIE_VERTEX_RADIUS * zoomScale;
	to.y -= dir.y * TRIE_VERTEX_RADIUS * zoomScale;
	from.x += dir.x * TRIE_VERTEX_RADIUS * zoomScale;
	from.y += dir.y * TRIE_VERTEX_RADIUS * zoomScale;

	ImVec2 p1 = ImVec2(to.x - dir.x * headSize - dir.y * headSize, to.y - dir.y * headSize + dir.x * headSize);
	ImVec2 p2 = ImVec2(to.x - dir.x * headSize + dir.y * headSize, to.y - dir.y * headSize - dir.x * headSize);
	draw_list->AddTriangleFilled(p1, p2, to, getColor(DEFAULT_EDGE_COL));

	to.x -= dir.x * headSize / 2;
	to.y -= dir.y * headSize / 2;
	from.x += dir.x * headSize / 2;
	from.y += dir.y * headSize / 2;

	draw_list->AddLine(from, to, getColor(DEFAULT_EDGE_COL), thickness);

}

void Trie::insertWord() {
	if (!nodes[curNode].children.count(word[curIdx])) {
		nodes[curNode].children[word[curIdx]] = (int)nodes.size();
		nodes.emplace_back();
		nodes.back().val = word[curIdx];
		parent.push_back(curNode);
	}
	curNode = nodes[curNode].children[word[curIdx++]];
	nodes[curNode].cnt++;
}

bool Trie::search(std::string word, bool prefixSearch) {
	int node = 0;
	for (auto it : word) {
		char current = it;
		if (!nodes[node].children.count(current))
			return false;
		node = nodes[node].children[current];
		updatedNodes.push(node);
	}
	return (nodes[node].endofword || prefixSearch);
}

void Trie::clear() {
	nodes.clear();
	parent.clear();
	nodes.push_back(Vertex());
	nodes[0].fixed = true;
	nodes[0].color = MARKED_VERT_COL;
	nodes[0].cnt = 1;
	parent.push_back(0);
}

Trie::Trie(std::string name, int& state, float& GuiScale, bool& settingsEnabled, int& colorMode)
	: GrandWindow(name, state, GuiScale, settingsEnabled, colorMode)
{
	nodes.push_back(Vertex());
	nodes[0].fixed = true;
	nodes[0].color = MARKED_VERT_COL;
	nodes[0].cnt = 1;
	parent.push_back(0);
}

Trie::~Trie()
{

}

void Trie::update()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);

	ImGui::Begin(getName().c_str(), NULL, main_flags);

	ImGui::PopStyleVar();

	drawWatermark();

	graphUpdate();

	if (updatedNodes.size() || curNode != -1 || deletedNodes.size()) {

		curTime += io->DeltaTime;

		while (curTime * (camFollow ? 0.7f : speed) >= TRIE_DELAY) {
			curTime -= TRIE_DELAY / (camFollow ? 0.7f : speed);

			if (updatedNodes.size() && nodes[updatedNodes.front()].color != DEF_VERT_COL) {
				if (nodes[updatedNodes.front()].endofword) {
					nodes[updatedNodes.front()].color = MARKED_VERT_COL;
				}
				else
				{
					nodes[updatedNodes.front()].color = DEF_VERT_COL;
				}
				updatedNodes.pop();

			}

			if (updatedNodes.size()) {

				if (deleting)
					deletedNodes.push(updatedNodes.front());

				if (updatedNodes.size() == 1 && !trueUpdate)
				{
					nodes[updatedNodes.front()].color = INCORRECT;
				}
				else
					nodes[updatedNodes.front()].color = UPDATED_NODE_COLOR;
				continue;
			}

			if (curNode != -1) {
				if (curIdx == word.size()) {
					nodes[curNode].color = MARKED_VERT_COL;
					nodes[curNode].endofword++;
					curIdx = -1;
					curNode = -1;
					word.clear();
				}
				else {
					if (curNode != 0)
						nodes[curNode].color = DEF_VERT_COL;
					if (nodes[curNode].endofword)
						nodes[curNode].color = MARKED_VERT_COL;
					insertWord();
					nodes[curNode].color = UPDATED_NODE_COLOR;
					continue;
				}
			}

			if (deletedNodes.size()) {
				if (nodes[deletedNodes.top()].color != DEF_VERT_COL) {

					int idx = deletedNodes.top();
					if (trueUpdate && idx == endw)
						nodes[idx].endofword--;
					if (nodes[idx].endofword) {
						nodes[idx].color = MARKED_VERT_COL;
					}
					else
					{
						nodes[idx].color = DEF_VERT_COL;
					}
					char val = nodes[idx].val[0];
					deletedNodes.pop();

					if (deletedNodes.size())
						nodes[deletedNodes.top()].color = UPDATED_NODE_COLOR;

					if (trueUpdate) {
						nodes[idx].cnt--;
						if (nodes[idx].cnt == 0) {
							if (deletedNodes.size())
								nodes[deletedNodes.top()].children.erase(val);
							else
								nodes[0].children.erase(val);
						}
					}

				}
				else {
					nodes[deletedNodes.top()].color = UPDATED_NODE_COLOR;
				}
				if (deletedNodes.size())
					continue;

			}
		}
	}
	else {
		disableButtons = false;
		trueUpdate = false;
		deleting = false;
	}
	if ((ImGui::IsWindowHovered() || movingCam) && ((ImGui::IsMouseDown(0) && cur_tool == 1) || ImGui::IsMouseDown(2))) {
		movingCam = true;
		camPos.x += io->MouseDelta.x / zoomScale;
		camPos.y += io->MouseDelta.y / zoomScale;
		camTarget.x += io->MouseDelta.x / zoomScale;
		camTarget.y += io->MouseDelta.y / zoomScale;
	}
	else if (!(ImGui::IsMouseDown(0) && cur_tool == 1) && !ImGui::IsMouseDown(2)) {
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
