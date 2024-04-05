#pragma once
#include "imgui.h"
#include <imgui_internal.h>
#include <string>

class DockingArea
{
private:
	// private fields:
	ImGuiWindowClass window_class;
	const std::string name;

public:

	DockingArea(std::string);
	~DockingArea();

	// public methods:
	void update();
	const std::string getName();

};

