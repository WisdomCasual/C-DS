#pragma once
#include <string>

class GrandWindow
{
private:

	std::string name;

public:

	int& state;
	float& scale;

	GrandWindow(std::string name, int& state, float& scale) : name(name), state(state), scale(scale) {};

	virtual void update() = 0;
	const std::string getName() { return name; };

};

