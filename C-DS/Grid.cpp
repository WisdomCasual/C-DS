#include "Grid.h"
#include <imgui_internal.h>

void Grid::updateMenuBar()
{

}

void Grid::controlsUpdate()
{
	ImVec2 controlsWinSize(std::min(400.f * scale, viewport->WorkSize.x - ImGui::GetStyle().WindowPadding.x), std::min(810.f * scale, viewport->WorkSize.y - ImGui::GetStyle().WindowPadding.y));
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

	ImGui::Text("Grid Size:");

	if (ImGui::SliderInt("X", &x_size, X_MIN, X_MAX, NULL, ImGuiSliderFlags_AlwaysClamp)) {
		clearVisited();
		if (!inbounds(start_pos.first, start_pos.second) || !inbounds(end_pos.first, end_pos.second)) {
			start_pos = { 0, 0 }, end_pos = { x_size - 1, y_size - 1 };
			is_obstacle[0][0] = false;
			is_obstacle[x_size - 1][y_size - 1] = false;
		}
	}
	if(ImGui::SliderInt("Y", &y_size, Y_MIN, Y_MAX, NULL, ImGuiSliderFlags_AlwaysClamp)) {
		clearVisited();
		if (!inbounds(start_pos.first, start_pos.second) || !inbounds(end_pos.first, end_pos.second)) {
			start_pos = { 0, 0 }, end_pos = { x_size - 1, y_size - 1 };
			is_obstacle[0][0] = false;
			is_obstacle[x_size - 1][y_size - 1] = false;
		}
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f));


	ImGui::Text("Tools:");
	ImGui::RadioButton("Move Camera", &cur_tool, 0);
	if (camTarget.x != 0 || camTarget.y != 0) {
		ImGui::SameLine();
		if (ImGui::Button("Reset"))
			camTarget = { 0, 0 };
	}
	ImGui::RadioButton("Set Start Position", &cur_tool, 1);
	ImGui::RadioButton("Set End Position", &cur_tool, 2);
	ImGui::RadioButton("Add Obstacles", &cur_tool, 3);
	ImGui::RadioButton("Remove Obstacles", &cur_tool, 4);
	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	ImGui::Text("Traversal Algorithms:");

	if (!cleared && activeAlgo == 0) {
		ImGui::SameLine();
		if (ImGui::Button("Reset"))
			clearVisited();
	}
	else if (activeAlgo != 0) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();

		ImGui::SameLine();

		if(paused){
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
		clearVisited();
		dfs_stack.push({ start_pos.first, start_pos.second });
	}

	if (ImGui::Button("BFS (Breadth First Search)")) {
		activeAlgo = 2;
		clearVisited(); 
		bfs_queue.push({ start_pos.first, start_pos.second });
	}

	if (ImGui::Button("Dijkstra")) {
		activeAlgo = 3;
		clearVisited();
		dijkstra_queue.push({ 0, { start_pos.first, start_pos.second } });
	}

	if (ImGui::Button("A* (A-star)")) {
		activeAlgo = 4;
		clearVisited();
		astar_queue.push(aStarNode(Node(start_pos), 0, g_val(Node(start_pos))));
	}

	ImGui::Checkbox("Diagonal Movement", &diagonal_movement);

	if (disabled) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	ImGui::SliderFloat("Speed", &speed, MIN_SPEED, MAX_SPEED, "%.1fx", ImGuiSliderFlags_AlwaysClamp);

	ImGui::End();
}

void Grid::gridUpdate()
{

	if (ImGui::IsWindowFocused() && ImGui::IsMouseDown(0) && cur_tool == 0) {
		camPos.x += io->MouseDelta.x;
		camPos.y += io->MouseDelta.y;
		camTarget.x += io->MouseDelta.x;
		camTarget.y += io->MouseDelta.y;
	}

	ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);

	float separator_size = 5 * scale;
	float cell_size = std::min({ 70 * scale, (viewport->WorkSize.x - separator_size * x_size) / x_size,
											  (viewport->WorkSize.y - separator_size * y_size) / y_size });

	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	ImVec2 s_pos(center.x + camPos.x - x_size * (cell_size + separator_size) / 2.f,
					    center.y + camPos.y - y_size * (cell_size + separator_size) / 2.f);
	ImVec2 cur_pos(s_pos);

	for (int y = 0; y < y_size; y++) {
		cur_pos.x = s_pos.x;
		for (int x = 0; x < x_size; x++) {
			if (ImGui::IsMouseDown(0) && io->MousePos.x >= cur_pos.x && io->MousePos.x <= cur_pos.x + cell_size && io->MousePos.y >= cur_pos.y && io->MousePos.y <= cur_pos.y + cell_size)
				useTool(x, y);

			draw_list->AddRectFilled(cur_pos, ImVec2(cur_pos.x + cell_size, cur_pos.y + cell_size), getColor(x, y));
			cur_pos.x += cell_size + separator_size;
		}
		cur_pos.y += cell_size + separator_size;
	}

}

void Grid::useTool(int x, int y)
{
	if (cur_tool == 0 || !ImGui::IsWindowFocused() || activeAlgo != 0)
		return;

	clearVisited();

	if (cur_tool == 1) {
		if (end_pos.first != x || end_pos.second != y) {
			is_obstacle[x][y] = 0;
			start_pos = { x, y };
		}
	}
	else if (cur_tool == 2) {
		if (start_pos.first != x || start_pos.second != y) {
			is_obstacle[x][y] = 0;
			end_pos = { x, y };
		}
	}
	else if (cur_tool == 3) {
		if((start_pos.first != x || start_pos.second != y) && (end_pos.first != x || end_pos.second != y))
			is_obstacle[x][y] = 1;
	}
	else if (cur_tool == 4) {
		if ((start_pos.first != x || start_pos.second != y) && (end_pos.first != x || end_pos.second != y))
			is_obstacle[x][y] = 0;
	}

}

ImU32 Grid::getColor(int x, int y)
{
	ImU32 col;

	if(vis[x][y] == 2)
		col = ImGui::GetColorU32(IM_COL32(50, 150, 150, 255)); // path
	else if(vis[x][y] == 1)
		col = ImGui::GetColorU32(IM_COL32(50, 150, 50, 255)); // visited
	else if(x == start_pos.first && y == start_pos.second)
		col = ImGui::GetColorU32(IM_COL32(50, 50, 150, 255)); // starting pos
	else if (x == end_pos.first && y == end_pos.second)
		col = ImGui::GetColorU32(IM_COL32(150, 50, 50, 255)); // end pos
	else if(is_obstacle[x][y])
		col = ImGui::GetColorU32(IM_COL32(50, 50, 50, 255)); // obstacle
	else
		col = ImGui::GetColorU32(IM_COL32(150, 150, 150, 255)); // default

	return col;
}

void Grid::clearVisited()
{
	if (cleared)
		return;

	while (!bfs_queue.empty())
		bfs_queue.pop();

	while (!dijkstra_queue.empty())
		dijkstra_queue.pop();

	while (!astar_queue.empty())
		astar_queue.pop();

	while (!dfs_stack.empty())
		dfs_stack.pop();

	cleared = true;
	for (int x = 0; x < X_MAX; x++)
		for (int y = 0; y < Y_MAX; y++)
			vis[x][y] = 0;
}

bool Grid::inbounds(int x, int y)
{
	return x >= 0 && x < x_size && y >= 0 && y < y_size;
}

double Grid::getCost(int x, int y)
{
	if (x != 0 && y != 0)
		return diagonal_cost;
	else
		return 1.0;
}

void Grid::dfs()
{

	if (!found && !dfs_stack.empty()) {

		std::pair<int, int> node = dfs_stack.top();
		vis[node.first][node.second] = 1;

		if (node == end_pos) {
			cur_node = node;
			found = true;
			return;
		}

		dfs_stack.pop();

		if (diagonal_movement) {
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					int new_x = node.first + dir[i];
					int new_y = node.second + dir[j];
					if (inbounds(new_x, new_y) && !is_obstacle[new_x][new_y] && vis[new_x][new_y] == 0) {
						if (dir[i] != 0 && dir[j] != 0 && is_obstacle[new_x][node.second] && is_obstacle[node.first][new_y])
							continue;
						vis[new_x][new_y] = -1;
						par[new_x][new_y] = node;
						dfs_stack.push({ new_x, new_y });
					}
				}
			}
		}
		else {
			for (int i = 0; i < 4; i++) {
				int new_x = node.first + dx[i];
				int new_y = node.second + dy[i];
				if (inbounds(new_x, new_y) && !is_obstacle[new_x][new_y] && vis[new_x][new_y] == 0) {
					vis[new_x][new_y] = -1;
					par[new_x][new_y] = node;
					dfs_stack.push({ new_x, new_y });
				}
			}
		}
	}
	else if (found && cur_node != start_pos) {
		vis[cur_node.first][cur_node.second] = 2;
		cur_node = par[cur_node.first][cur_node.second];
	}
	else {
		if(found)
			vis[cur_node.first][cur_node.second] = 2;
		activeAlgo = 0;
		curTime = 0;
		found = false;
	}
}

void Grid::bfs()
{

	if (!found && !bfs_queue.empty()) {

		std::pair<int, int> node = bfs_queue.front();
		vis[node.first][node.second] = 1;
		bfs_queue.pop();

		if (node == end_pos) {
			cur_node = node;
			found = true;
			return;
		}

		if (diagonal_movement) {
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					int new_x = node.first + dir[i];
					int new_y = node.second + dir[j];
					if (inbounds(new_x, new_y) && !is_obstacle[new_x][new_y] && vis[new_x][new_y] == 0) {
						if (dir[i] != 0 && dir[j] != 0 && is_obstacle[new_x][node.second] && is_obstacle[node.first][new_y])
							continue;
						vis[new_x][new_y] = -1;
						par[new_x][new_y] = node;
						bfs_queue.push({ new_x, new_y });
					}
				}
			}
		}
		else {
			for (int i = 0; i < 4; i++) {
				int new_x = node.first + dx[i];
				int new_y = node.second + dy[i];
				if (inbounds(new_x, new_y) && !is_obstacle[new_x][new_y] && vis[new_x][new_y] == 0) {
					vis[new_x][new_y] = -1;
					par[new_x][new_y] = node;
					bfs_queue.push({ new_x, new_y });
				}
			}
		}
	}
	else if (found && cur_node != start_pos) {
		vis[cur_node.first][cur_node.second] = 2;
		cur_node = par[cur_node.first][cur_node.second];
	}
	else {
		if (found)
			vis[cur_node.first][cur_node.second] = 2;
		activeAlgo = 0;
		curTime = 0;
		found = false;
	}

}

void Grid::dijkstra()
{
	if (!found && !dijkstra_queue.empty()) {

		double distance = dijkstra_queue.top().first;
		std::pair<int, int> node = dijkstra_queue.top().second;
		vis[node.first][node.second] = 1;
		dijkstra_queue.pop();

		if (node == end_pos) {
			cur_node = node;
			found = true;
			return;
		}

		if (diagonal_movement) {
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					int new_x = node.first + dir[i];
					int new_y = node.second + dir[j];
					if (inbounds(new_x, new_y) && !is_obstacle[new_x][new_y] && vis[new_x][new_y] == 0) {
						if (dir[i] != 0 && dir[j] != 0 && is_obstacle[new_x][node.second] && is_obstacle[node.first][new_y])
							continue;
						vis[new_x][new_y] = -1;
						par[new_x][new_y] = node;
						dijkstra_queue.push({ distance - getCost(dir[i], dir[j]) , {new_x, new_y}});
					}
				}
			}
		}
		else {
			for (int i = 0; i < 4; i++) {
				int new_x = node.first + dx[i];
				int new_y = node.second + dy[i];
				if (inbounds(new_x, new_y) && !is_obstacle[new_x][new_y] && vis[new_x][new_y] == 0) {
					vis[new_x][new_y] = -1;
					par[new_x][new_y] = node;
					dijkstra_queue.push({ distance - getCost(dx[i], dy[i]) , { new_x, new_y } });
				}
			}
		}
	}
	else if (found && cur_node != start_pos) {
		vis[cur_node.first][cur_node.second] = 2;
		cur_node = par[cur_node.first][cur_node.second];
	}
	else {
		if (found)
			vis[cur_node.first][cur_node.second] = 2;
		activeAlgo = 0;
		curTime = 0;
		found = false;
	}
}

double Grid::g_val(Node cur)
{
	double dx = abs(end_pos.first - cur.x);
	double dy = abs(end_pos.second - cur.y);
	if (!diagonal_movement) {
		return dx+dy;
	}
	else {
		if (dx > dy) std::swap(dx, dy);
		return (dy + dx * (diagonal_cost-1));
	}
}

void Grid::a_star()
{
	if (!found && !astar_queue.empty()) {

		double distance = astar_queue.top().curNode.cost - astar_queue.top().gVal;
		Node node = astar_queue.top().curNode.coordinates;
		vis[node.x][node.y] = 1;
		astar_queue.pop();

		if (node.x == end_pos.first && node.y == end_pos.second) {
			cur_node.first = node.x;
			cur_node.second = node.y;
			found = true;
			return;
		}

		if (diagonal_movement) {
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {

					Node new_node = { node.x + dir[i], node.y + dir[j] };
					if (inbounds(new_node.x, new_node.y) && !is_obstacle[new_node.x][new_node.y] && vis[new_node.x][new_node.y] == 0) {
						if (dir[i] != 0 && dir[j] != 0 && is_obstacle[new_node.x][node.y] && is_obstacle[node.x][new_node.y])
							continue;
						vis[new_node.x][new_node.y] = -1;
						par[new_node.x][new_node.y].first = node.x;
						par[new_node.x][new_node.y].second = node.y;
						double g = g_val(new_node);
						double cost = getCost(dir[i], dir[j]);
						astar_queue.push(aStarNode(new_node, distance + cost + g, g));
					}
				}
			}
		}
		else {
			for (int i = 0; i < 4; i++) {
				Node new_node = {node.x + dx[i], node.y + dy[i]};
				if (inbounds(new_node.x, new_node.y) && !is_obstacle[new_node.x][new_node.y] && vis[new_node.x][new_node.y] == 0) {
					vis[new_node.x][new_node.y] = -1;
					par[new_node.x][new_node.y].first = node.x;
					par[new_node.x][new_node.y].second = node.y;
					double g = g_val(new_node);
					astar_queue.push(aStarNode(new_node, distance + 1 + g , g));
				}
			}
		}
	}
	else if (found && cur_node != start_pos) {
		vis[cur_node.first][cur_node.second] = 2;
		cur_node = par[cur_node.first][cur_node.second];
	}
	else {
		if (found)
			vis[cur_node.first][cur_node.second] = 2;
		activeAlgo = 0;
		curTime = 0;
		found = false;
	}
}

Grid::Grid(std::string name, int& state, float& scale, bool& settingEnabled)
	: GrandWindow(name, state, scale, settingsEnabled)
{
	io = &ImGui::GetIO(); (void)io;
}

Grid::~Grid()
{

}

void Grid::update()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);

	ImGui::Begin(getName().c_str(), NULL, main_flags);

	ImGui::PopStyleVar();

	gridUpdate();

	if (!paused && activeAlgo != 0) {

		curTime += io->DeltaTime;

		while (curTime * speed >= DELAY_TIME) {
			cleared = false;
			curTime -= DELAY_TIME / speed;

			if (activeAlgo == 1) {
				dfs();
			}
			else if (activeAlgo == 2) {
				bfs();
			}
			else if (activeAlgo == 3) {
				dijkstra();
			}
			else if (activeAlgo == 4) {
				a_star();
			}

		}
	}

	ImGui::End();

	camPos.x += (camTarget.x - camPos.x) * 10.f * io->DeltaTime;
	camPos.y += (camTarget.y - camPos.y) * 10.f * io->DeltaTime;
	
	controlsUpdate();
}
