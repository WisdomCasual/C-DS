#pragma once
#include <string>

class GrandWindow
{
private:

	std::string name;

public:

	int& state;
	float& scale;
	bool& settingsEnabled;

	GrandWindow(std::string name, int& state, float& scale, bool& settingsEnabled) : name(name), state(state), scale(scale), settingsEnabled(settingsEnabled){};

	virtual void update() = 0;
	const std::string getName() { return name; };

};

