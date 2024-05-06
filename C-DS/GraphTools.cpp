#include "GraphTools.h"
#include <imgui_internal.h>
#include <sstream>
#include <iostream>

void GraphTools::pointToNode(const std::string u, ImU32 color)
{
	const ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	const float headSize = 12.f * zoomScale;
	const float length = 30.f * zoomScale;
	auto& node = nodes[u];

	ImVec2 from = ImVec2(center.x + (camPos.x + node.x) * zoomScale, center.y + (camPos.y + node.y) * zoomScale - VERTEX_RADIUS - length);
	ImVec2 to = ImVec2(center.x + (camPos.x + node.x) * zoomScale, center.y + (camPos.y + node.y) * zoomScale - VERTEX_RADIUS - headSize);

	draw_list->AddLine(from, to, color, 5.f * zoomScale);
	to.y += headSize / 2.f;

	ImVec2 p1 = ImVec2(to.x + headSize, to.y - headSize);
	ImVec2 p2 = ImVec2(to.x - headSize, to.y - headSize);

	draw_list->AddTriangleFilled(p1, p2, to, color);

}

void GraphTools::updateMenuBar()
{

}

void GraphTools::controlsUpdate()
{
	ImVec2 controlsWinSize(std::min(450.f * GuiScale, viewport->WorkSize.x - ImGui::GetStyle().WindowPadding.x), std::min(1040.f * GuiScale, viewport->WorkSize.y - 2 * ImGui::GetStyle().WindowPadding.y));
	ImVec2 controlsWinPos(viewport->Size.x - controlsWinSize.x - ImGui::GetStyle().WindowPadding.x, viewport->Size.y - controlsWinSize.y - ImGui::GetStyle().WindowPadding.y);
	bool disabled = false;

	ImGui::SetNextWindowSize(controlsWinSize);
	ImGui::SetNextWindowPos(controlsWinPos);

	ImGui::Begin("Controls", NULL, controls_flags);

	if (ImGui::Button("Back"))
		state = 0;

	if (activeAlgo != 0) {
		disabled = true;
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	ImGui::Text("Tools:");

	if (!cleared) {
		ImGui::SameLine();
		if (ImGui::Button("Reset Graph"))
			clearStates();
	}

	ImGui::RadioButton("Move Nodes", &cur_tool, 0);
	ImGui::RadioButton("Move Camera", &cur_tool, 1);
	if (camTarget.x != 0 || camTarget.y != 0) {
		ImGui::SameLine();
		if (ImGui::Button("Reset"))
			camTarget = { 0, 0 };
	}

	if (ImGui::RadioButton("View Adjacent", &cur_tool, 4)) {
		clearStates();
	}
	ImGui::RadioButton("Set Starting Node", &cur_tool, 2);
	if (startNode.size()) {
		ImGui::SameLine();
		if (ImGui::Button("Remove##Start"))
			startNode.clear();
	}
	ImGui::RadioButton("Set Ending Node", &cur_tool, 3);
	if (endNode.size()) {
		ImGui::SameLine();
		if (ImGui::Button("Remove##End"))
			endNode.clear();
	}
	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	if (ImGui::RadioButton("Directed", &directed, 0))
		clearStates();

	ImGui::SameLine();

	if(ImGui::RadioButton("Undirected", &directed, 1))
		clearStates();

	ImGui::Text("Graph Data:   [u], [v], [w]");

	ImGui::InputTextMultiline(" ", graphText, IM_ARRAYSIZE(graphText), ImVec2(-FLT_MIN, 300));

	std::string curStr;
	char* curChar = graphText;
	std::map<std::string, Vertex> temp_nodes;
	std::map<std::pair<std::string, std::string>, Edge> temp_edges;
	std::map<std::string, std::set<std::pair<std::string, std::pair<int, bool>>>> temp_adj;

	do {
		if (*curChar == '\n' || *curChar == '\0') {
			std::stringstream ss(curStr);
			std::string u, v;
			int w;
			if (!(ss >> u)) {

			}
			else if (!(ss >> v)) {
				temp_nodes[u] = nodes[u];
				temp_adj[u];
			}
			else if (!(ss >> w)) {
				temp_nodes[u] = nodes[u];
				temp_nodes[v] = nodes[v];

				if (temp_edges.count({ u, v })) {
					curStr.clear();
					continue;
				}

				auto& newEdge = temp_edges[{u, v}];
				newEdge.color = edges[{u, v}].color;
				newEdge.weighted = false;
				newEdge.w = 0;

				temp_adj[u].insert({ v, {0, false} });
				if(!directed)
					temp_adj[v].insert({ u, {0, false} });
			}
			else {
				auto& u_node = nodes[u];
				auto& v_node = nodes[v];
				temp_nodes[u] = u_node;
				temp_nodes[v] = v_node;

				if (temp_edges.count({ u, v })) {
					curStr.clear();
					continue;
				}

				auto& newEdge = temp_edges[{u, v}];
				auto& oldEdge = edges[{u, v}];

				if (edges.count({ u, v })) {
					newEdge.pos = oldEdge.pos;
					newEdge.color = oldEdge.color;
				}
				else
					newEdge.pos = ImVec2((u_node.x + v_node.x) / 2, (u_node.y + v_node.y) / 2);

				newEdge.weighted = true;
				newEdge.w = w;

				temp_adj[u].insert({ v, {w, true} });
				if (!directed)
					temp_adj[v].insert({ u, {w, true} });
			}

			curStr.clear();
		}
		curStr.push_back(*curChar);
	} while (*curChar++ != '\0');

	nodes = temp_nodes;
	edges = temp_edges;
	adj = temp_adj;


	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	ImGui::Text("Traversal Algorithms:");

	if (activeAlgo != 0) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();

		ImGui::SameLine();

		if (paused) {
			if (ImGui::Button("Resume"))
				paused = false;
		}
		else {
			if (ImGui::Button("Pause"))
				paused = true;
		}

		ImGui::SameLine();

		if (ImGui::Button("Stop")) {
			activeAlgo = 0;
			found = false;
			paused = false;
			curTime = 0;
		}

		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	if (ImGui::Button("DFS (Depth First Search)")) {
		activeAlgo = 1;
		clearStates();
		//dfs_stack.push({ start_pos.first, start_pos.second });
	}

	if (ImGui::Button("BFS (Breadth First Search)")) {
		activeAlgo = 2;
		clearStates();
		//bfs_queue.push({ start_pos.first, start_pos.second });
	}

	if (ImGui::Button("Dijkstra")) {
		activeAlgo = 3;
		clearStates();
		//dijkstra_queue.push({ 0, { start_pos.first, start_pos.second } });
	}


	if (disabled) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	ImGui::Checkbox("Camera Follow", &camFollow);

	if (camFollow) {
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}
	ImGui::SliderFloat("Speed", &speed, MIN_SPEED, MAX_SPEED, "%.1fx", ImGuiSliderFlags_AlwaysClamp);
	if (camFollow) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}
	ImGui::End();
}

float GraphTools::calcDist(float x1, float y1, float x2, float y2)
{
	return sqrtf((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void GraphTools::clearStates()
{
	if (cleared)
		return;

	//while (!bfs_queue.empty())
	//	bfs_queue.pop();

	//while (!dijkstra_queue.empty())
	//	dijkstra_queue.pop();

	//while (!astar_queue.empty())
	//	astar_queue.pop();

	//while (!dfs_stack.empty())
	//	dfs_stack.pop();

	cleared = true;

	viewAdjacent.clear();

	for (auto& node : nodes) 
		node.second.color = DEFAULT_VERT_COL;

	for (auto& edge : edges)
		edge.second.color = DEFAULT_EDGE_COL;

}

ImU32 GraphTools::ContrastingColor(ImU32 col)
{
	ImVec4 ret = ImGui::ColorConvertU32ToFloat4(col);

	ret.x = std::min(ret.x + 50.f, 255.f);
	ret.y = std::min(ret.y + 50.f, 255.f);
	ret.z = std::min(ret.z + 50.f, 255.f);

	return ImGui::ColorConvertFloat4ToU32(ret);
}

void GraphTools::DrawEdge(ImDrawList* draw_list, const std::string u, const std::string v, Edge& edge) {

	ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);

	ImVec2 from = ImVec2(center.x + (camPos.x + nodes[u].x) * zoomScale, center.y + (camPos.y + nodes[u].y) * zoomScale);
	ImVec2 to = ImVec2(center.x + (camPos.x + nodes[v].x) * zoomScale, center.y + (camPos.y + nodes[v].y) * zoomScale);

	const ImU32 color = edge.color;
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


	if (directed) {
		ImVec2 p1 = ImVec2(to.x - dir.x * headSize - dir.y * headSize, to.y - dir.y * headSize + dir.x * headSize);
		ImVec2 p2 = ImVec2(to.x - dir.x * headSize + dir.y * headSize, to.y - dir.y * headSize - dir.x * headSize);
		draw_list->AddTriangleFilled(p1, p2, to, color);
	}

	to.x -= dir.x * headSize / 2;
	to.y -= dir.y * headSize / 2;
	from.x += dir.x * headSize / 2;
	from.y += dir.y * headSize / 2;

	draw_list->AddLine(from, to, color, thickness);

	if (edge.weighted) {
		
		length = calcDist(from.x, from.y, to.x, to.y);

		std::string w_label = std::to_string(edge.w);

		ImVec2 textSize = ImGui::CalcTextSize(w_label .c_str());
		
		if (directed) {
			to.x -= dir.x * length / 4.f;
			to.y -= dir.y * length / 4.f;
		}
		else {
			to.x -= dir.x * length / 2.f;
			to.y -= dir.y * length / 2.f;
		}
		to.x -= textSize.x / 2.f;
		to.y -= textSize.y / 2.f;

		to.x += dir.y * (textSize.x / 2.f + 12.f * zoomScale);
		to.y -= dir.x * (textSize.y / 2.f + 10.f * zoomScale);

		auto& pos = edge.pos;

		pos.x += (to.x - pos.x - center.x - camPos.x * zoomScale) * 40.f * io->DeltaTime;
		pos.y += (to.y - pos.y - center.y - camPos.y * zoomScale) * 40.f * io->DeltaTime;

		draw_list->AddText(ImVec2(center.x + pos.x + camPos.x * zoomScale, center.y + pos.y + camPos.y * zoomScale), ContrastingColor(edge.color), w_label.c_str());
	}

}

void GraphTools::graphUpdate()
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

	for (auto& edge : edges) {

		const std::string& u_str = edge.first.first;
		const std::string& v_str = edge.first.second;

		if (u_str == v_str)
			continue;

		auto& u = nodes[u_str];
		auto& v = nodes[v_str];

		float dx = v.x - u.x;
		float dy = v.y - u.y;
		float d = calcDist(u.x, u.y, v.x, v.y);

		float force = k * (d - EDGE_LENGTH);

		u.fx += force * dx / d;
		u.fy += force * dy / d;
		v.fx -= force * dx / d;
		v.fy -= force * dy / d;

	}

	for (auto& node : nodes) {
		ImVec2 pos;
		pos.x = center.x + (camPos.x + node.second.x) * zoomScale;
		pos.y = center.y + (camPos.y + node.second.y) * zoomScale;

		float dist = calcDist(pos.x, pos.y, io->MousePos.x, io->MousePos.y);
		
		if (ImGui::IsMouseDown(0) && dragging.empty() && dist <= VERTEX_RADIUS && ImGui::IsWindowFocused() && cur_tool == 0) {
			dragging = node.first;
		}
		else if ((!ImGui::IsWindowFocused() || !ImGui::IsMouseDown(0 || cur_tool != 0)) && !dragging.empty()) {
			dragging.clear();
		}

		if (ImGui::IsMouseDoubleClicked(0) && dist <= VERTEX_RADIUS && ImGui::IsWindowFocused() && cur_tool == 0) {
			node.second.fixed = !node.second.fixed;
		}

		if (ImGui::IsMouseDown(0) && !leftClickPressed && dist <= VERTEX_RADIUS && ImGui::IsWindowHovered() && ImGui::IsWindowFocused() && (cur_tool == 2 || cur_tool == 3)) {
			(cur_tool == 2 ? startNode : endNode) = node.first;
			leftClickPressed = true;
		}
		else if(!ImGui::IsMouseDown(0)){
			leftClickPressed = false;
		}


		if (ImGui::IsMouseDown(0) && dist <= VERTEX_RADIUS && viewAdjacent != node.first && ImGui::IsWindowFocused() && cur_tool == 4) {
			clearStates();
			cleared = false;
			node.second.color = ADJ_ROOT_COL;
			viewAdjacent = node.first;
			for (auto& child : adj[node.first]) {
				nodes[child.first].color = ADJ_CHILD_COL;
				if(edges.count({ node.first, child.first }))
					edges[{node.first, child.first}].color = ADJ_EDGE_COL;
				if (edges.count({ child.first, node.first }))
					edges[{child.first, node.first}].color = ADJ_EDGE_COL;
			}
		}

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

	for (auto& edge : edges)
		DrawEdge(draw_list, edge.first.first, edge.first.second, edge.second);

	if (startNode.size()) {
		pointToNode(startNode, START_POINT_COL);
	}

	if (endNode.size()) {
		pointToNode(endNode, END_POINT_COL);
	}

	for (auto& node : nodes) {
		ImVec2 textCenter = ImGui::CalcTextSize(node.first.c_str());
		textCenter.x /= 2.f;
		textCenter.y /= 2.f;
		draw_list->AddCircleFilled(ImVec2(center.x + (camPos.x + node.second.x) * zoomScale, center.y + (camPos.y + node.second.y) * zoomScale), VERTEX_RADIUS, node.second.color);
		if (node.second.fixed)
			draw_list->AddCircle(ImVec2(center.x + (camPos.x + node.second.x) * zoomScale, center.y + (camPos.y + node.second.y) * zoomScale), VERTEX_RADIUS, FIXED_NODE_COLOR, 100, 5.f * zoomScale);
		draw_list->AddText(ImVec2(center.x + (camPos.x + node.second.x) * zoomScale - textCenter.x, center.y + (camPos.y + node.second.y) * zoomScale - textCenter.y), ContrastingColor(node.second.color), node.first.c_str());
	}


}

void GraphTools::dfs()
{

}

void GraphTools::bfs()
{

}

void GraphTools::dijkstra()
{

}

GraphTools::GraphTools(std::string name, int& state, float& GuiScale, bool& settingEnabled)
	: GrandWindow(name, state, GuiScale, settingsEnabled)
{
	io = &ImGui::GetIO(); (void)io;
}

GraphTools::~GraphTools()
{

}

void GraphTools::update()
{

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);

	ImGui::Begin(getName().c_str(), NULL, main_flags);

	ImGui::PopStyleVar();

	graphUpdate();

	if (!paused && activeAlgo != 0) {

		curTime += io->DeltaTime;

		while (curTime * (camFollow ? 0.7f : speed) >= DELAY_TIME) {
			cleared = false;
			curTime -= DELAY_TIME / (camFollow ? 0.7f : speed);

			if (activeAlgo == 1) {
				dfs();
			}
			else if (activeAlgo == 2) {
				bfs();
			}
			else if (activeAlgo == 3) {
				dijkstra();
			}

		}
	}

	if (ImGui::IsWindowHovered() && ((ImGui::IsMouseDown(0) && cur_tool == 1) || ImGui::IsMouseDown(2))) {
		camPos.x += io->MouseDelta.x / zoomScale;
		camPos.y += io->MouseDelta.y / zoomScale;
		camTarget.x += io->MouseDelta.x / zoomScale;
		camTarget.y += io->MouseDelta.y / zoomScale;
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