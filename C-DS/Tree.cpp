#include "Tree.h"
#include <imgui_internal.h>

ImU32 Tree::ContrastingColor(ImU32 col)
{
	ImVec4 ret = ImGui::ColorConvertU32ToFloat4(col);

	ret.x = std::min(ret.x + 50.f, 255.f);
	ret.y = std::min(ret.y + 50.f, 255.f);
	ret.z = std::min(ret.z + 50.f, 255.f);

	return ImGui::ColorConvertFloat4ToU32(ret);
}

void Tree::controlsUpdate()
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

	ImGui::RadioButton("Move Nodes", &cur_tool, 0);

	ImGui::RadioButton("Move Camera", &cur_tool, 1);
	if (camTarget.x != 0 || camTarget.y != 0) {
		ImGui::SameLine();
		if (ImGui::Button("Reset##Move_Cam"))
			camTarget = { 0, 0 };
	}

	ImGui::RadioButton("View Component", &cur_tool, 2);

	if (viewComponent.size()) {
		ImGui::SameLine();
		if (ImGui::Button("Reset##View_Component"))
			viewComponent.clear();
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));



	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));



	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	ImGui::InputText("##Enter_Node", add_node_text, IM_ARRAYSIZE(add_node_text));

	//ImGui::SameLine();

	int itemCnt = 0;
	static int cur_p = 0;
	const char** items = new const char* [nodes.size() + 1];

	items[itemCnt++] = "";

	for (auto node : nodes) {
		char* temparr = new char[node.first.size() + 1];
		for (int i = 0; i < node.first.size(); i++)
			temparr[i] = node.first[i];
		temparr[node.first.size()] = '\0';

		items[itemCnt++] = temparr;
	}

	if (root == "\0") {
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	ImGui::Combo("##u", &cur_p, items, itemCnt);

	if (root == "\0") {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	std::string cur_p_str = items[cur_p];

	for (int i = 1; i < itemCnt; i++)
		delete[] items[i];

	if ((root != "\0" && cur_p == 0) || add_node_text[0] == '\0') {
		disabled = true;
	}
	if (disabled) {
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	if (ImGui::Button("Insert")) {
		std::string curNode;
		char* ptr = add_node_text;
		while (*ptr != '\0') {
			curNode.push_back(*ptr);
			ptr++;
		}

		if (curNode.size() > 0) {

			if (root == "\0") {
				root = cur_p_str = curNode;
				nodes[root].color = ROOT_NODE_COLOR;
				nodes[root].fixed = true;
			}
			addNode(curNode, cur_p_str);
			curNode.clear();
		}
		cur_p = 0;
		memset(add_node_text, 0, sizeof add_node_text);
	}

	if (disabled) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	disabled = false;

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	ImGui::SliderFloat("Speed", &speed, DSU_MIN_SPEED, DSU_MAX_SPEED, "%.1fx", ImGuiSliderFlags_AlwaysClamp);

	ImGui::End();
}

float Tree::calcDist(float x1, float y1, float x2, float y2)
{
	return sqrtf((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void Tree::graphUpdate()
{

	const ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	const float cf = 0.55f; // center attraction
	const float k = 3.f; // Spring constant
	const float c = 8000.f; // Repulsion constant
	const float dampening = 0.85f; // Dampening factor

	for (auto& node : nodes) {

		auto& u = node.second;
		u.fx -= cf * u.x;
		u.fy -= cf * u.y;

		for (auto& otherNode : nodes) {
			if (node.first != otherNode.first) {
				auto& v = otherNode.second;

				float d = calcDist(u.x, u.y, v.x, v.y);
				float dx = v.x - u.x;
				float dy = v.y - u.y;
				float force = c / (d * d);

				u.fx -= force * dx;
				u.fy -= force * dy;
			}
		}
	}

	for (auto& node : nodes) {
		if (node.first != parent[node.first]) {
			auto& u = nodes[node.first];
			auto& v = nodes[parent[node.first]];

			float dx = v.x - u.x;
			float dy = v.y - u.y;
			float d = calcDist(u.x, u.y, v.x, v.y);

			float force = k * (d - EDGE_LENGTH);

			u.fx += force * dx / d;
			u.fy += force * dy / d;
			v.fx -= force * dx / d;
			v.fy -= force * dy / d;
		}
	}

	for (auto& node : nodes) {
		ImVec2 pos;
		pos.x = center.x + (camPos.x + node.second.x) * zoomScale;
		pos.y = center.y + (camPos.y + node.second.y) * zoomScale;

		float dist = calcDist(pos.x, pos.y, io->MousePos.x, io->MousePos.y);

		if (ImGui::IsMouseDown(0) && dragging.empty() && dist <= VERTEX_RADIUS && ImGui::IsWindowHovered() && cur_tool == 0) {
			dragging = node.first;
		}
		else if ((!ImGui::IsWindowFocused() || !ImGui::IsMouseDown(0 || cur_tool != 0)) && !dragging.empty()) {
			dragging.clear();
		}
		/*
		if (ImGui::IsMouseDoubleClicked(0) && dist <= VERTEX_RADIUS && ImGui::IsWindowHovered() && cur_tool == 0) {
			node.second.fixed = !node.second.fixed;
		}
		*/
		if (ImGui::IsMouseDown(0) && dist <= VERTEX_RADIUS && ImGui::IsWindowHovered() && cur_tool == 2) {
			if (!leftClickPressed) {
				viewComponent = node.first;
			}
		}
		/*
		if (node.second.searching) {
			node.second.color = VIS_VERT_COL;
		}
		else if (sameGroup(node.first, viewComponent)) {
			node.second.color = COMP_VERT_COL;
		}
		else
			node.second.color = DEFAULT_VERT_COL;
			*/

		if (dragging == node.first) {
			node.second.fx = node.second.fy = 0;
			node.second.x += io->MouseDelta.x / zoomScale;
			node.second.y += io->MouseDelta.y / zoomScale;
			continue;
		}
		if (node.second.fixed) {
			node.second.fx = node.second.fy = 0;
			continue;
		}
		node.second.x += node.second.fx * io->DeltaTime;
		node.second.y += node.second.fy * io->DeltaTime;
		node.second.fx *= dampening;
		node.second.fy *= dampening;
	}

	if (ImGui::IsMouseDown(0) && dragging.empty() && ImGui::IsWindowFocused() && cur_tool == 0) {
		dragging = char(2);
	}

	for (auto& node : nodes)
		if (parent.count(node.first))
			drawEdge(draw_list, node.first, parent[node.first]);

	for (auto& node : nodes) {
		ImVec2 textCenter = ImGui::CalcTextSize(node.first.c_str());
		textCenter.x /= 2.f;
		textCenter.y /= 2.f;

		draw_list->AddCircleFilled(ImVec2(center.x + (camPos.x + node.second.x) * zoomScale, center.y + (camPos.y + node.second.y) * zoomScale), VERTEX_RADIUS, node.second.color);
		if (node.second.fixed)
			draw_list->AddCircle(ImVec2(center.x + (camPos.x + node.second.x) * zoomScale, center.y + (camPos.y + node.second.y) * zoomScale), VERTEX_RADIUS, FIXED_NODE_COLOR, 100, 5.f * zoomScale);
		draw_list->AddText(ImVec2(center.x + (camPos.x + node.second.x) * zoomScale - textCenter.x, center.y + (camPos.y + node.second.y) * zoomScale - textCenter.y), ContrastingColor(node.second.color), node.first.c_str());
	}

	if (ImGui::IsMouseDown(0))
		leftClickPressed = true;
	else
		leftClickPressed = false;

}

void Tree::drawEdge(ImDrawList* draw_list, const std::string u, const std::string v)
{
	ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);

	ImVec2 from = ImVec2(center.x + (camPos.x + nodes[v].x) * zoomScale, center.y + (camPos.y + nodes[v].y) * zoomScale);
	ImVec2 to = ImVec2(center.x + (camPos.x + nodes[u].x) * zoomScale, center.y + (camPos.y + nodes[u].y) * zoomScale);

	//const ImU32 color = edge.color;
	const ImU32 color = ImGui::GetColorU32(IM_COL32(200, 200, 200, 255));
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
	draw_list->AddTriangleFilled(p1, p2, to, color);

	to.x -= dir.x * headSize / 2;
	to.y -= dir.y * headSize / 2;
	from.x += dir.x * headSize / 2;
	from.y += dir.y * headSize / 2;

	draw_list->AddLine(from, to, color, thickness);

}

void Tree::addNode(std::string u, std::string p)
{
	nodes[u];
	parent[u] = p;
}
Tree::Tree(std::string name, int& state, float& GuiScale, bool& settingsEnabled, int& colorMode)
	: GrandWindow(name, state, GuiScale, settingsEnabled, colorMode)
{

}

Tree::~Tree()
{

}

void Tree::update()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);

	ImGui::Begin(getName().c_str(), NULL, main_flags);

	ImGui::PopStyleVar();

	graphUpdate();

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

	updateCam();

	ImGui::End();

	controlsUpdate();

}
