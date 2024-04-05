#include "DockingArea.h"

DockingArea::DockingArea(std::string name)
    : name(name)
{
    window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDockingOverMe | ImGuiDockNodeFlags_NoDockingOverOther | ImGuiDockNodeFlags_NoDockingSplitOther;
}

DockingArea::~DockingArea()
{

}

void DockingArea::update()
{
    ImGui::SetNextWindowClass(&window_class);
    ImGui::Begin(name.c_str(), NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImGuiID dockSpace = ImGui::GetID("dockSpace");
    ImGui::DockSpace(dockSpace);
    ImGui::End();
}

const std::string DockingArea::getName()
{
    return name;
}
