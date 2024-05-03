#include "GraphTools.h"
#include <imgui_internal.h>
#include <iostream>
#include <iomanip>

void GraphTools::updateMenuBar()
{

}

void GraphTools::controlsUpdate()
{
	ImVec2 controlsWinSize(std::min(315.f * scale, viewport->WorkSize.x - ImGui::GetStyle().WindowPadding.x), std::min(650.f * scale, viewport->WorkSize.y - ImGui::GetStyle().WindowPadding.y));
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

	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	ImGui::Text("Tools:");
	ImGui::RadioButton("Move Nodes", &cur_tool, 0);
	ImGui::RadioButton("Move Camera", &cur_tool, 1);
	if (camTarget.x != 0 || camTarget.y != 0) {
		ImGui::SameLine();
		if (ImGui::Button("Reset"))
			camTarget = { 0, 0 };
	}

	ImGui::RadioButton("Set Starting Node", &cur_tool, 2);
	ImGui::RadioButton("Set Ending Node", &cur_tool, 3);
	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	ImGui::RadioButton("Directed", &directed, 0);
	ImGui::SameLine();
	ImGui::RadioButton("Undirected", &directed, 1);

	ImGui::Text("Graph Data:");

	static char text[int(1e4)];
	ImGui::InputTextMultiline(" ", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, 300));




	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	ImGui::Text("Traversal Algorithms:");

	if (!cleared && activeAlgo == 0) {
		ImGui::SameLine();
		if (ImGui::Button("Reset")) {
			//clearVisited();
		}
	}
	else if (activeAlgo != 0) {
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
		//clearVisited();
		//dfs_stack.push({ start_pos.first, start_pos.second });
	}

	if (ImGui::Button("BFS (Breadth First Search)")) {
		activeAlgo = 2;
		//clearVisited();
		//bfs_queue.push({ start_pos.first, start_pos.second });
	}

	if (ImGui::Button("Dijkstra")) {
		activeAlgo = 3;
		//clearVisited();
		//dijkstra_queue.push({ 0, { start_pos.first, start_pos.second } });
	}

	if (disabled) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	ImGui::SliderFloat("Speed", &speed, MIN_SPEED, MAX_SPEED, "%.1fx", ImGuiSliderFlags_AlwaysClamp);

	ImGui::End();
}

float GraphTools::calcDist(float x1, float y1, float x2, float y2)
{
	return sqrtf((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void GraphTools::DrawEdge(ImDrawList* drawList, ImVec2 from, ImVec2 to) {

	const ImU32 color = ImGui::GetColorU32(IM_COL32(200, 200, 200, 255));
	const float thickness = 5.f * scale;

	ImVec2 dir = ImVec2(to.x - from.x, to.y - from.y);
	float length = calcDist(from.x, from.y, to.x, to.y);
	dir.x /= length;
	dir.y /= length;

	const float headSize = 15.f * scale;
	to.x -= dir.x * VERTEX_RADIUS;
	to.y -= dir.y * VERTEX_RADIUS;
	from.x += dir.x * VERTEX_RADIUS;
	from.y += dir.y * VERTEX_RADIUS;

	if (directed) {
		ImVec2 p1 = ImVec2(to.x - dir.x * headSize - dir.y * headSize, to.y - dir.y * headSize + dir.x * headSize);
		ImVec2 p2 = ImVec2(to.x - dir.x * headSize + dir.y * headSize, to.y - dir.y * headSize - dir.x * headSize);
		drawList->AddTriangleFilled(p1, p2, to, color);
	}

	to.x -= dir.x * headSize / 2;
	to.y -= dir.y * headSize / 2;
	from.x += dir.x * headSize / 2;
	from.y += dir.y * headSize / 2;

	drawList->AddLine(from, to, color, thickness);

}

void GraphTools::graphUpdate()
{

	ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	const float cf = 0.5f; // center attraction
	const float k = 0.75f; // Spring constant
	const float c = 5000.f; // Repulsion constant
	const float dampening = 0.9f; // Dampening factor

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
		auto& u = nodes[edge.u];
		auto& v = nodes[edge.v];

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
		pos.x = center.x + camPos.x + node.second.x;
		pos.y = center.y + camPos.y + node.second.y;

		float dist = calcDist(pos.x, pos.y, io->MousePos.x, io->MousePos.y);
		
		if (ImGui::IsMouseDown(0) && dragging.empty() && dist <= VERTEX_RADIUS && ImGui::IsWindowFocused() && cur_tool == 0) {
			dragging = node.first;
		}
		else if ((!ImGui::IsWindowFocused() || !ImGui::IsMouseDown(0 || cur_tool != 0)) && !dragging.empty()) {
			dragging.clear();
		}

		if (dragging == node.first) {
			node.second.fx = node.second.fy = 0;
			node.second.x += io->MouseDelta.x;
			node.second.y += io->MouseDelta.y;
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
		DrawEdge(draw_list, ImVec2(center.x + camPos.x + nodes[edge.u].x, center.y + camPos.y + nodes[edge.u].y), ImVec2(center.x + camPos.x + nodes[edge.v].x, center.y + camPos.y + nodes[edge.v].y));

	for (auto& node : nodes) {
		ImVec2 textCenter = ImGui::CalcTextSize(node.first.c_str());
		textCenter.x /= 2.f;
		textCenter.y /= 2.f;
		draw_list->AddCircleFilled(ImVec2(center.x + camPos.x + node.second.x, center.y + camPos.y + node.second.y), VERTEX_RADIUS, ImGui::GetColorU32(IM_COL32(150, 150, 150, 255)));
		draw_list->AddText(ImVec2(center.x + camPos.x + node.second.x - textCenter.x, center.y + camPos.y + node.second.y - textCenter.y), ImGui::GetColorU32(IM_COL32(255, 255, 255, 255)), node.first.c_str());
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

GraphTools::GraphTools(std::string name, int& state, float& scale, bool& settingEnabled)
	: GrandWindow(name, state, scale, settingsEnabled)
{
	io = &ImGui::GetIO(); (void)io;


	// for testing only:
	edges.insert(Edge("1", "2"));
	edges.insert(Edge("2", "1"));

	edges.insert(Edge("1", "5"));
	edges.insert(Edge("3", "4"));

	nodes["1"];
	nodes["2"];
	nodes["3"];
	nodes["4"];
	nodes["5"];
	nodes["6"];

	nodes["baka"].x = -100;
	nodes["baka"].y = 100;

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

		while (curTime * speed >= DELAY_TIME) {
			cleared = false;
			curTime -= DELAY_TIME / speed;

			//if (activeAlgo == 1) {
			//	dfs();
			//}
			//else if (activeAlgo == 2) {
			//	bfs();
			//}
			//else if (activeAlgo == 3) {
			//	dijkstra();
			//}
			//else if (activeAlgo == 4) {
			//	a_star();
			//}

		}
	}

	if (ImGui::IsWindowFocused() && ImGui::IsMouseDown(0) && cur_tool == 1) {
		camPos.x += io->MouseDelta.x;
		camPos.y += io->MouseDelta.y;
		camTarget.x += io->MouseDelta.x;
		camTarget.y += io->MouseDelta.y;
	}

	camPos.x += (camTarget.x - camPos.x) * 10.f * io->DeltaTime;
	camPos.y += (camTarget.y - camPos.y) * 10.f * io->DeltaTime;

	ImGui::End();

	controlsUpdate();
}
