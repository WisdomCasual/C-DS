#pragma once
#include <string>

class GrandWindow
{
private:

	std::string name;

public:

	int& state;
	float& GuiScale;
	float zoomScale = 1.f;
	bool& settingsEnabled;

	GrandWindow(std::string name, int& state, float& GuiScale, bool& settingsEnabled) : name(name), state(state), GuiScale(GuiScale), settingsEnabled(settingsEnabled){};

	virtual void update() = 0;
	const std::string getName() { return name; };

};

