#include "GrandWindow.h"


void GrandWindow::drawWatermark(float opacity) const {
	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[5]);

	float shade = abs(colorMode - 0.8f);
	ImGui::TextColored(ImVec4(shade, shade, shade, opacity), "C-");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.3f, 0.6f, 1.0f, opacity), "DS");
	ImGui::PopFont();
};

void GrandWindow::updateCam(float minZoomScale, float maxZoomScale) {

	updateCenter();

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

void GrandWindow::autoZoom(float newZoom) {
	targetZoom = newZoom;
	autoZooming = true;
	camTarget = { 0, 0 };
};