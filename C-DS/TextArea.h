#pragma once
#include "imgui.h"
#include <imgui_internal.h>
#include <string>

class TextArea
{
	// private fields:
	ImGuiWindowClass window_class;
	const std::string name;

public:

	TextArea(std::string);
	~TextArea();

	// public methods:
	void update();
	const std::string getName();

};