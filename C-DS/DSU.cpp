#include "DSU.h"
#include <imgui_internal.h>

ImU32 DSU::ContrastingColor(ImU32 col)
{
	ImVec4 ret = ImGui::ColorConvertU32ToFloat4(col);

	ret.x = std::min(ret.x + 50.f, 255.f);
	ret.y = std::min(ret.y + 50.f, 255.f);
	ret.z = std::min(ret.z + 50.f, 255.f);

	return ImGui::ColorConvertFloat4ToU32(ret);
}

void DSU::updateMenuBar()
{

}

void DSU::controlsUpdate()
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

	ImGui::Text("Union by: (Rank)");

	bool mergingDisable = false;
	if (isMerging) {
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		mergingDisable = true;
	}

	ImGui::RadioButton("None", &rank_type, 0);
	ImGui::RadioButton("Node Count", &rank_type, 1);
	ImGui::RadioButton("Estimated Tree Height", &rank_type, 2);

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	ImGui::Checkbox("Path Compression", &pathCompression);

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	ImGui::InputText("##Enter_Node", add_node_text, IM_ARRAYSIZE(add_node_text));

	ImGui::SameLine();

	if (ImGui::Button("Insert")) {
		std::string curNode;
		char* ptr = add_node_text;
		while (*ptr != '\0') {
			if (*ptr == ',') {
				if (curNode.size() > 0) {
					addNode(curNode);
					curNode.clear();
				}
			}
			else
				curNode.push_back(*ptr);

			ptr++;
		}
		if (curNode.size() > 0) {
			addNode(curNode);
			curNode.clear();
		}

		memset(add_node_text, 0, sizeof add_node_text);

	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	int itemCnt = 0;
	static int current_u = 0;
	static int current_v = 0;
	const char** items = new const char* [nodes.size() + 1];

	items[itemCnt++] = "";

	for (auto node : nodes) {
		char* temparr = new char[node.first.size() + 1];
		for (int i = 0; i < node.first.size(); i++)
			temparr[i] = node.first[i];
		temparr[node.first.size()] = '\0';

		items[itemCnt++] = temparr;
	}

	if (ImGui::Combo("##u", &current_u, items, itemCnt)) {
		current_v = 0;
	}

	const std::string cur_u_str = items[current_u];

	for (int i = 1; i < itemCnt; i++)
		delete[] items[i];

	itemCnt = 1;

	for (auto node : nodes) {
		if (!sameGroup(cur_u_str, node.first)) { // <- not same component
			char* temparr = new char[node.first.size() + 1];
			for (int i = 0; i < node.first.size(); i++)
				temparr[i] = node.first[i];
			temparr[node.first.size()] = '\0';

			items[itemCnt++] = temparr;
		}
	}

	if (current_u == 0) {
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	ImGui::Combo("##v", &current_v, items, itemCnt);

	if (current_u == 0) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}
	const std::string cur_v_str = items[current_v];

	for (int i = 1; i < itemCnt; i++)
		delete[] items[i];

	delete[] items;

	bool disabled_union = false;

	if (current_u == 0 || current_v == 0) {
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		disabled_union = true;
	}

	ImGui::SameLine();

	if (ImGui::Button("Union")) {
		isMerging = true;
		node_u = cur_u_str;
		node_v = cur_v_str;
		cur_node_u = cur_u_str;
		cur_node_v = cur_v_str;
		nodes[node_u].searching = true;
		nodes[node_v].searching = true;
		current_u = current_v = 0;
	}

	if (disabled_union) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}
	if (mergingDisable) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	ImGui::SliderFloat("Speed", &speed, MIN_SPEED, MAX_SPEED, "%.1fx", ImGuiSliderFlags_AlwaysClamp);

	ImGui::End();
}

float DSU::calcDist(float x1, float y1, float x2, float y2)
{
	return sqrtf((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void DSU::graphUpdate()
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

		if (ImGui::IsMouseDoubleClicked(0) && dist <= VERTEX_RADIUS && ImGui::IsWindowHovered() && cur_tool == 0) {
			node.second.fixed = !node.second.fixed;
		}

		if (ImGui::IsMouseDown(0) && dist <= VERTEX_RADIUS && ImGui::IsWindowHovered() && cur_tool == 2) {
			if (!leftClickPressed) {
				viewComponent = node.first;
			}
		}

		if (node.second.searching) {
			node.second.color = VIS_VERT_COL;
		}
		else if (sameGroup(node.first, viewComponent)) {
			node.second.color = COMP_VERT_COL;
		}
		else
			node.second.color = DEFAULT_VERT_COL;

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

void DSU::drawEdge(ImDrawList* draw_list, const std::string u, const std::string v)
{
	ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);

	ImVec2 from = ImVec2(center.x + (camPos.x + nodes[u].x) * zoomScale, center.y + (camPos.y + nodes[u].y) * zoomScale);
	ImVec2 to = ImVec2(center.x + (camPos.x + nodes[v].x) * zoomScale, center.y + (camPos.y + nodes[v].y) * zoomScale);

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

bool DSU::sameGroup(const std::string u, const std::string v) {
	bool prevComp = pathCompression;
	pathCompression = false;

	std::string set_u = find(u);
	std::string set_v = find(v);

	pathCompression = prevComp;

	return set_u == set_v;
}

void DSU::mergeGroup(const std::string u, const std::string v)
{

	std::string root_u = find(u);
	std::string root_v = find(v);

	if (root_u == root_v)
		return;

	if (rank_type == BY_HEIGHT) {
		if (st_height[root_u] < st_height[root_v])
			parent[root_u] = root_v, st_size[root_v] += st_size[root_u];
		else if (st_height[root_u] > st_height[root_v])
			parent[root_v] = root_u, st_size[root_u] += st_size[root_v];
		else
			parent[root_v] = root_u, st_height[root_u] = st_height[root_v] + 1, st_size[root_u] += st_size[root_v];
	}
	else if(rank_type == BY_SIZE){
		if (st_size[root_u] > st_size[root_v]) {
			parent[root_v] = root_u;
			st_size[root_u] += st_size[root_v];
			st_height[root_v] = std::max(st_height[root_v], st_height[root_u] + 1);
		}
		else {
			parent[root_u] = root_v;
			st_size[root_v] += st_size[root_u];
			st_height[root_u] = std::max(st_height[root_u], st_height[root_v] + 1);
		}
	}
	else {
		parent[root_u] = root_v;
		st_size[root_v] += st_size[root_u];
		st_height[root_u] = std::max(st_height[root_u], st_height[root_v] + 1);
	}

}

const std::string DSU::find(const std::string s)
{
	if (s == parent[s])
		return s;

	std::string fnd = find(parent[s]);

	if (pathCompression)
		parent[s] = fnd;

	return fnd;
}

void DSU::addNode(std::string u)
{
	nodes[u];
	parent[u] = u;
	st_size[u] = 1;
	st_height[u] = 1;
}

DSU::DSU(std::string name, int& state, float& GuiScale, bool& settingEnabled)
	: GrandWindow(name, state, GuiScale, settingsEnabled)
{
	io = &ImGui::GetIO(); (void)io;
}

DSU::~DSU()
{

}

void DSU::update()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);

	ImGui::Begin(getName().c_str(), NULL, main_flags);

	ImGui::PopStyleVar();

	graphUpdate();

	if (isMerging) {

		curTime += io->DeltaTime;

		while (curTime * speed >= DELAY) {

			curTime -= DELAY / speed;

			if (cur_node_u != parent[cur_node_u] || cur_node_v != parent[cur_node_v]) {
				nodes[cur_node_u].searching = false;
				cur_node_u = parent[cur_node_u];
				nodes[cur_node_u].searching = true;

				nodes[cur_node_v].searching = false;
				cur_node_v = parent[cur_node_v];
				nodes[cur_node_v].searching = true;
			}
			else {
				curTime = 0;
				isMerging = false;
				nodes[cur_node_u].searching = false;
				nodes[cur_node_v].searching = false;

				mergeGroup(node_u, node_v);
			}
		}

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
