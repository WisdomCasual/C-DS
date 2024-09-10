#pragma once
#include <string>
#include <imgui.h>

class GrandWindow
{
private:

	std::string name;

public:

	int& state;
	float& GuiScale;
	float zoomScale = 1.f, targetZoom = 1.f;
	bool& settingsEnabled;
	int& colorMode;

	GrandWindow(std::string name, int& state, float& GuiScale, bool& settingsEnabled, int& colorMode) : name(name), state(state), GuiScale(GuiScale), settingsEnabled(settingsEnabled), colorMode(colorMode){};

	void inline drawWatermark() const {
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);

		float shade = abs(colorMode - 0.8f);
		ImGui::TextColored(ImVec4(shade, shade, shade, 0.7f), "C-");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.3f, 0.6f, 1.0f, 0.7f), "DS");
		ImGui::PopFont();
	};
	virtual void update() = 0;
	const inline std::string getName() { return name; };

};

