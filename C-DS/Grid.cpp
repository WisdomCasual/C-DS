#include "Grid.h"
#include <imgui_internal.h>
#include <iostream>


void Grid::controlsUpdate()
{
	ImVec2 controlsWinSize(std::min(450.f * GuiScale, viewport->WorkSize.x - ImGui::GetStyle().WindowPadding.x), std::min(855.f * GuiScale, viewport->WorkSize.y - 2 * ImGui::GetStyle().WindowPadding.y));
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

	ImGui::Text("Grid Size:");

	if (ImGui::SliderInt("X", &x_size, X_MIN, X_MAX, NULL, ImGuiSliderFlags_AlwaysClamp)) {
		clearVisited();
		if (!inbounds(start_pos.first, start_pos.second) || !inbounds(end_pos.first, end_pos.second)) {
			start_pos = { 0, 0 }, end_pos = { x_size - 1, y_size - 1 };
			is_obstacle[0][0] = false;
			is_obstacle[x_size - 1][y_size - 1] = false;
		}
		camTarget = { 0, 0 };
		autoZoom(std::min({ 1.f, std::min(viewport->WorkSize.x / x_size, viewport->WorkSize.y / y_size) / (CELL_SIZE + SEPARATOR_SIZE) - 0.01f }));
	}
	if(ImGui::SliderInt("Y", &y_size, Y_MIN, Y_MAX, NULL, ImGuiSliderFlags_AlwaysClamp)) {
		clearVisited();
		if (!inbounds(start_pos.first, start_pos.second) || !inbounds(end_pos.first, end_pos.second)) {
			start_pos = { 0, 0 }, end_pos = { x_size - 1, y_size - 1 };
			is_obstacle[0][0] = false;
			is_obstacle[x_size - 1][y_size - 1] = false;
		}
		camTarget = { 0, 0 };
		autoZoom(std::min({ 1.f, std::min(viewport->WorkSize.x / x_size, viewport->WorkSize.y / y_size) / (CELL_SIZE + SEPARATOR_SIZE) - 0.01f }));
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));


	ImGui::Text("Tools:");
	ImGui::RadioButton("Move Camera", &cur_tool, 0);
	if (camTarget.x != 0 || camTarget.y != 0) {
		ImGui::SameLine();
		if (ImGui::Button("Reset##Cam"))
			camTarget = { 0, 0 };
	}
	ImGui::RadioButton("Set Start Position", &cur_tool, 1);
	ImGui::RadioButton("Set End Position", &cur_tool, 2);
	ImGui::RadioButton("Add Obstacles", &cur_tool, 3);
	ImGui::RadioButton("Remove Obstacles", &cur_tool, 4);

	if (obstaclesCnt > 0) {
		ImGui::SameLine();
		if (ImGui::Button("Remove All")) {
			obstaclesCnt = 0;
			for (int x = 0; x < X_MAX; x++)
				for (int y = 0; y < Y_MAX; y++)
					is_obstacle[x][y] = false;
		}
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f * GuiScale));

	ImGui::Text("Traversal Algorithms:");

	if (!cleared && activeAlgo == 0) {
		ImGui::SameLine();
		if (ImGui::Button("Reset##Grid"))
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
		cost[start_pos.first][start_pos.second] = 0.0;
		astar_queue.push(aStarNode(Node(start_pos), 0, g_val(Node(start_pos))));
	}

	ImGui::Checkbox("Diagonal Movement", &diagonal_movement);

	if (disabled) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	if (ImGui::Checkbox("Camera Follow", &camFollow)) {
		if (camFollow)
			autoZoom(1.4f);
	}

	if (camFollow) {
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}
	ImGui::SliderFloat("Speed", &speed, GRID_MIN_SPEED, GRID_MAX_SPEED, "%.1fx", ImGuiSliderFlags_AlwaysClamp);
	if (camFollow) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}


	ImGui::End();
}

void Grid::st_line_eq(ImVec2 s_pos, float cell_size)
{
	int x = int((io->MousePos.x - s_pos.x) / (cell_size)),
		y = int((io->MousePos.y - s_pos.y) / cell_size);
	if (x < 0 || y < 0 || x >= x_size || y >= y_size)
		return;

	if (prev_x == -1)
		prev_x = x, prev_y = y;
	int dx = x - prev_x, dy = y - prev_y;


	if (dx == dy && dx == 0) {
		useTool(x, y);
		return;
	}

	int* start = nullptr, * otherDimension = nullptr, end = 0;
	int targ_x = std::min(x, prev_x);
	int targ_y = std::min(y, prev_y);
	int c = 0;
	float slope = 0;

	if ((abs(dx) > abs(dy))) {
		start = &targ_x;
		otherDimension = &targ_y;
		targ_x = std::min(x, prev_x);
		end = std::max(x, prev_x);
		slope = (float)dy / dx;
		c = y - int(round(x * slope));
	}
	else {
		start = &targ_y;
		otherDimension = &targ_x;
		targ_y = std::min(y, prev_y);
		end = std::max(y, prev_y);
		slope = (float)dx / dy;
		c = x - int(round(y * slope));
	}

	for (; *start <= end; (*start)++) {
		*otherDimension = int(round((*start) * slope)) + c;
		useTool(targ_x, targ_y);
	}
	prev_x = x, prev_y = y;
}

void Grid::gridUpdate()
{

	ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);

	float separator_size = std::max(SEPARATOR_SIZE * zoomScale, 1.f);
	float cell_size = CELL_SIZE * zoomScale;

	ImDrawList* draw_list = ImGui::GetWindowDrawList();


	ImVec2 s_pos(center.x + camPos.x * zoomScale - x_size * (cell_size + separator_size) / 2.f,
					    center.y + camPos.y * zoomScale - y_size * (cell_size + separator_size) / 2.f);
	ImVec2 cur_pos(s_pos);

	draw_list->AddRectFilled(ImVec2(cur_pos.x - separator_size, cur_pos.y - separator_size), ImVec2(cur_pos.x + (cell_size + separator_size) * x_size, cur_pos.y + (cell_size + separator_size) * y_size), getColor(OUTLINE_COL), GRID_ROUNDNESS * zoomScale);

	if (ImGui::IsMouseDown(0)) {
		st_line_eq(s_pos, cell_size + separator_size);
	}
	else {
		prev_x = prev_y = -1;
	}

	for (int y = 0; y < y_size; y++) {
		cur_pos.x = s_pos.x;
		for (int x = 0; x < x_size; x++) {

			draw_list->AddRectFilled(cur_pos, ImVec2(cur_pos.x + cell_size, cur_pos.y + cell_size), getCellColor(x, y), GRID_ROUNDNESS * zoomScale);
			cur_pos.x += cell_size + separator_size;
		}
		cur_pos.y += cell_size + separator_size;
	}

}

void Grid::useTool(int x, int y)
{
	if (cur_tool == 0 || !ImGui::IsWindowHovered() || activeAlgo != 0)
		return;

	clearVisited();

	if (cur_tool == 1) {
		if (end_pos.first != x || end_pos.second != y) {
			if (is_obstacle[x][y])
				obstaclesCnt--;
			is_obstacle[x][y] = 0;
			start_pos = { x, y };
		}
	}
	else if (cur_tool == 2) {
		if (start_pos.first != x || start_pos.second != y) {
			if (is_obstacle[x][y])
				obstaclesCnt--;
			is_obstacle[x][y] = 0;
			end_pos = { x, y };
		}
	}
	else if (cur_tool == 3) {
		if ((start_pos.first != x || start_pos.second != y) && (end_pos.first != x || end_pos.second != y)) {
			if (!is_obstacle[x][y])
				obstaclesCnt++;
			is_obstacle[x][y] = 1;
		}
	}
	else if (cur_tool == 4) {
		if ((start_pos.first != x || start_pos.second != y) && (end_pos.first != x || end_pos.second != y)) {
			if (is_obstacle[x][y])
				obstaclesCnt--;
			is_obstacle[x][y] = 0;
		}
	}

}

ImU32 Grid::getColor(int color_code) {

	switch (color_code) {
	case PATH_CELL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(50, 200, 200, 255)) : ImGui::GetColorU32(IM_COL32(50, 150, 150, 255));
	case VISITED_CELL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(50, 200, 50, 255)) : ImGui::GetColorU32(IM_COL32(50, 150, 50, 255));
	case START_POS_CELL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(50, 50, 200, 255)) : ImGui::GetColorU32(IM_COL32(50, 50, 150, 255));
	case END_POS_CELL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(200, 50, 50, 255)) : ImGui::GetColorU32(IM_COL32(150, 50, 50, 255));
	case OBSTACLE_CELL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(50, 50, 50, 255)) : ImGui::GetColorU32(IM_COL32(50, 50, 50, 255));
	case INQUE_CELL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(220, 220, 0, 255)) : ImGui::GetColorU32(IM_COL32(200, 200, 0, 255));
	case EMPTY_CELL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(200, 200, 200, 255)) : ImGui::GetColorU32(IM_COL32(150, 150, 150, 255));
	case OUTLINE_COL:
		return colorMode ? ImGui::GetColorU32(IM_COL32(0, 0, 0, 255)) : ImGui::GetColorU32(IM_COL32(40, 40, 40, 255));


	default:
		return ImGui::GetColorU32(IM_COL32(255, 0, 255, 255));
	}

}

ImU32 Grid::getCellColor(int x, int y)
{

	if(is_obstacle[x][y])
		return getColor(OBSTACLE_CELL);

	if(x == start_pos.first && y == start_pos.second)
		return getColor(START_POS_CELL);

	if (x == end_pos.first && y == end_pos.second)
		return getColor(END_POS_CELL);

	return getColor(vis[x][y]);
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
			vis[x][y] = EMPTY_CELL, cost[x][y] = FLT_MAX;
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

void Grid::followCell(int x, int y)
{
	float separator_size = std::max(SEPARATOR_SIZE * zoomScale, 1.f);
	float cell_size = CELL_SIZE * zoomScale;
	camTarget = ImVec2(x_size * (cell_size + separator_size) / 2.f - (cell_size + separator_size) * x,
					   y_size * (cell_size + separator_size) / 2.f - (cell_size + separator_size) * y);
	camTarget.x /= zoomScale;
	camTarget.y /= zoomScale;
}

void Grid::dfs()
{

	if (!found && !dfs_stack.empty()) {
		while (!found && !dfs_stack.empty()) {
			std::pair<int, int> node = dfs_stack.top();
			vis[node.first][node.second] = VISITED_CELL;

			if (camFollow)
				followCell(node.first, node.second);

			if (node == end_pos) {
				cur_node = node;
				found = true;
				return;
			}

			if (diagonal_movement) {
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {

						if (i == 2 && j == 2) dfs_stack.pop();

						int new_x = node.first + dir[i];
						int new_y = node.second + dir[j];
						if (inbounds(new_x, new_y) && !is_obstacle[new_x][new_y] && vis[new_x][new_y] == EMPTY_CELL) {
							if (dir[i] != 0 && dir[j] != 0 && is_obstacle[new_x][node.second] && is_obstacle[node.first][new_y])
								continue;
							vis[new_x][new_y] = INQUE_CELL;
							par[new_x][new_y] = node;
							dfs_stack.push({ new_x, new_y });

							return;
						}
					}
				}
			}
			else {
				for (int i = 0; i < 4; i++) {

					if (i == 3) dfs_stack.pop();

					int new_x = node.first + dx[i];
					int new_y = node.second + dy[i];
					if (inbounds(new_x, new_y) && !is_obstacle[new_x][new_y] && vis[new_x][new_y] == EMPTY_CELL) {
						vis[new_x][new_y] = INQUE_CELL;
						par[new_x][new_y] = node;
						dfs_stack.push({ new_x, new_y });

						return;
					}
				}
			}
		}
	}
	else if (found && cur_node != start_pos) {
		vis[cur_node.first][cur_node.second] = PATH_CELL;
		if (camFollow)
			followCell(cur_node.first, cur_node.second);
		cur_node = par[cur_node.first][cur_node.second];
	}
	else {
		if (found) {
			vis[cur_node.first][cur_node.second] = PATH_CELL;
			if (camFollow)
				followCell(cur_node.first, cur_node.second);
		}
		activeAlgo = 0;
		curTime = 0;
		found = false;
	}
}

void Grid::bfs()
{

	if (!found && !bfs_queue.empty()) {

		std::pair<int, int> node = bfs_queue.front();
		vis[node.first][node.second] = VISITED_CELL;
		bfs_queue.pop();

		if(camFollow)
			followCell(node.first, node.second);

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
					if (inbounds(new_x, new_y) && !is_obstacle[new_x][new_y] && vis[new_x][new_y] == EMPTY_CELL) {
						if (dir[i] != 0 && dir[j] != 0 && is_obstacle[new_x][node.second] && is_obstacle[node.first][new_y])
							continue;
						vis[new_x][new_y] = INQUE_CELL;
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
				if (inbounds(new_x, new_y) && !is_obstacle[new_x][new_y] && vis[new_x][new_y] == EMPTY_CELL) {
					vis[new_x][new_y] = INQUE_CELL;
					par[new_x][new_y] = node;
					bfs_queue.push({ new_x, new_y });
				}
			}
		}
	}
	else if (found && cur_node != start_pos) {
		if(camFollow)
			followCell(cur_node.first, cur_node.second);
		vis[cur_node.first][cur_node.second] = PATH_CELL;
		cur_node = par[cur_node.first][cur_node.second];
	}
	else {
		if (found) {
			vis[cur_node.first][cur_node.second] = PATH_CELL;
			if (camFollow)
				followCell(cur_node.first, cur_node.second);
		}
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

		if (camFollow)
			followCell(node.first, node.second);

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
					if (inbounds(new_x, new_y) && !is_obstacle[new_x][new_y] && vis[new_x][new_y] == EMPTY_CELL) {
						if (dir[i] != 0 && dir[j] != 0 && is_obstacle[new_x][node.second] && is_obstacle[node.first][new_y])
							continue;
						vis[new_x][new_y] = INQUE_CELL;
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
				if (inbounds(new_x, new_y) && !is_obstacle[new_x][new_y] && vis[new_x][new_y] == EMPTY_CELL) {
					vis[new_x][new_y] = INQUE_CELL;
					par[new_x][new_y] = node;
					dijkstra_queue.push({ distance - getCost(dx[i], dy[i]) , { new_x, new_y } });
				}
			}
		}
	}
	else if (found && cur_node != start_pos) {
		vis[cur_node.first][cur_node.second] = PATH_CELL;
		if (camFollow)
			followCell(cur_node.first, cur_node.second);
		cur_node = par[cur_node.first][cur_node.second];
	}
	else {
		if (found) {
			vis[cur_node.first][cur_node.second] = PATH_CELL;
			if (camFollow)
				followCell(cur_node.first, cur_node.second);
		}
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
		return (dy + dx * (diagonal_cost-1.0));
	}
}

void Grid::a_star()
{
	if (!found && !astar_queue.empty()) {
		Node node = astar_queue.top().curNode.coordinates;
		double distance = cost[node.x][node.y];
		vis[node.x][node.y] = VISITED_CELL;
		astar_queue.pop();

		if (camFollow)
			followCell(node.x, node.y);

		if (node.x == end_pos.first && node.y == end_pos.second) {
			cur_node.first = node.x;
			cur_node.second = node.y;
			found = true;
			return;
		}

		if (diagonal_movement) {

			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					if (i == j && dir[i] == 0)
						continue;
					Node new_node = {node.x + dir[i], node.y + dir[j] };
					if (inbounds(new_node.x, new_node.y) && !is_obstacle[new_node.x][new_node.y]) {
						if (dir[i] != 0 && dir[j] != 0 && is_obstacle[new_node.x][node.y] && is_obstacle[node.x][new_node.y])
							continue;
						double new_dist = distance + getCost(dir[i], dir[j]);
						if (new_dist < cost[new_node.x][new_node.y]) {

							cost[new_node.x][new_node.y] = new_dist;
							par[new_node.x][new_node.y].first = node.x;
							par[new_node.x][new_node.y].second = node.y;
						}
						if (vis[new_node.x][new_node.y] == EMPTY_CELL) {
							vis[new_node.x][new_node.y] = INQUE_CELL;
							double g = g_val(new_node);
							astar_queue.push(aStarNode(new_node, new_dist, g));
						}
						
					}
				}
			}

		}
		else {
			for (int i = 0; i < 4; i++) {
				Node new_node = {node.x + dx[i], node.y + dy[i]};
				double new_dist = distance + 1;
				if (inbounds(new_node.x, new_node.y) && !is_obstacle[new_node.x][new_node.y]) {
					if (new_dist < cost[new_node.x][new_node.y]) {
						cost[new_node.x][new_node.y] = new_dist;
						par[new_node.x][new_node.y].first = node.x;
						par[new_node.x][new_node.y].second = node.y;
					}
					if (vis[new_node.x][new_node.y] == EMPTY_CELL) {
						vis[new_node.x][new_node.y] = INQUE_CELL;
						double g = g_val(new_node);
						astar_queue.push(aStarNode(new_node, new_dist, g));
					}
				}
			}
		}
	}
	else if (found && cur_node != start_pos) {
		vis[cur_node.first][cur_node.second] = PATH_CELL;
		if (camFollow)
			followCell(cur_node.first, cur_node.second);
		cur_node = par[cur_node.first][cur_node.second];
	}
	else {
		if (found) {
			vis[cur_node.first][cur_node.second] = PATH_CELL;
			if (camFollow)
				followCell(cur_node.first, cur_node.second);
		}
		activeAlgo = 0;
		curTime = 0;
		found = false;
	}
}

Grid::Grid(std::string name, int& state, float& GuiScale, bool& settingsEnabled, int& colorMode)
	: GrandWindow(name, state, GuiScale, settingsEnabled, colorMode)
{
	clearVisited();
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

	drawWatermark();

	gridUpdate();

	if (!paused && activeAlgo != 0) {

		curTime += io->DeltaTime;

		while (curTime * (camFollow ? 0.7f : speed) >= GRID_DELAY_TIME) {
			cleared = false;
			curTime -= GRID_DELAY_TIME / (camFollow ? 0.7f : speed);

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
