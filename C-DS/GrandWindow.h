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
	ImVec2 camPos = { 0, 500.f * GuiScale }, camTarget = { 0, 0 };
	bool autoZooming = false;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	const ImGuiIO* io = &ImGui::GetIO();
	

	GrandWindow(std::string name, int& state, float& GuiScale, bool& settingsEnabled, int& colorMode) : name(name), state(state), GuiScale(GuiScale), settingsEnabled(settingsEnabled), colorMode(colorMode){};

	void inline drawWatermark(float opacity = 0.7f) const {
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);

		float shade = abs(colorMode - 0.8f);
		ImGui::TextColored(ImVec4(shade, shade, shade, opacity), "C-");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.3f, 0.6f, 1.0f, opacity), "DS");
		ImGui::PopFont();
	};

	void inline updateCam(float minZoomScale = 0.5f, float maxZoomScale = 3.0f) {


		const ImVec2 center(viewport->WorkPos.x + viewport->WorkSize.x / 2.f, viewport->WorkPos.y + viewport->WorkSize.y / 2.f);

		camPos.x += (camTarget.x - camPos.x) * 10.f * io->DeltaTime;
		camPos.y += (camTarget.y - camPos.y) * 10.f * io->DeltaTime;

		if (ImGui::IsWindowHovered() && io->MouseWheel != 0.0f) {
			targetZoom += io->MouseWheel * 0.15f;
			targetZoom = std::min(std::max(targetZoom, minZoomScale), maxZoomScale);
			autoZooming = false;
		}

		if (abs(zoomScale - targetZoom) <= 0.005f) return;

		if (!autoZooming) {
			camTarget.x -= (io->MousePos.x - center.x) / zoomScale;
			camTarget.y -= (io->MousePos.y - center.y) / zoomScale;
			camPos.x -= (io->MousePos.x - center.x) / zoomScale;
			camPos.y -= (io->MousePos.y - center.y) / zoomScale;
		}

		zoomScale += (targetZoom - zoomScale) * 10.f * io->DeltaTime;
		zoomScale = std::min(std::max(zoomScale, minZoomScale), maxZoomScale);

		if (!autoZooming) {
			camTarget.x += (io->MousePos.x - center.x) / zoomScale;
			camTarget.y += (io->MousePos.y - center.y) / zoomScale;
			camPos.x += (io->MousePos.x - center.x) / zoomScale;
			camPos.y += (io->MousePos.y - center.y) / zoomScale;
		}

	}

	void inline autoZoom(float newZoom) {
		targetZoom = newZoom;
		autoZooming = true;
		camTarget = { 0, 0 };
	};

	virtual void update() = 0;

	const inline std::string getName() { return name; };

};

