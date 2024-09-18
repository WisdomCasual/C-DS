#pragma once
#include <string>
#include <imgui.h>


class GrandWindow
{
private:

	std::string name;
	bool autoZooming = false;
	float targetZoom = 1.0f;

protected:

	int& state;
	float& GuiScale;
	float zoomScale = 1.0f;
	bool& settingsEnabled;
	int& colorMode;
	float center_x_percent = 0.5f, center_y_percent = 0.5f;
	ImVec2 camPos = { 0.0f, 1000.f * GuiScale }, camTarget = { 0.0f, 0.0f };
	ImVec2 center = { 0, 0 };

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	const ImGuiIO* io = &ImGui::GetIO();

	GrandWindow(std::string name, int& state, float& GuiScale, bool& settingsEnabled, int& colorMode) : name(name), state(state), GuiScale(GuiScale), settingsEnabled(settingsEnabled), colorMode(colorMode){};

	void drawWatermark(float opacity = 0.7f) const;
	void updateCam(float minZoomScale = 0.5f, float maxZoomScale = 3.0f);
	void autoZoom(float newZoom);
	void inline updateCenter() {
		// updates the center to the center of the current window
		center = ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x * center_x_percent, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y * center_y_percent);
	};

public:

	virtual void update() = 0;

	const inline std::string getName() { return name; };

};

