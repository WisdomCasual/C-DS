#include "CodeVisualizer.h"

void CodeVisualizer::updateMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New", "Ctrl+N")) {}
            if (ImGui::MenuItem("Open", "Ctrl+O")) {}
            if (ImGui::MenuItem("Save", "Ctrl+S")) {}
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Cut", "Ctrl+X")) {}
            if (ImGui::MenuItem("Copy", "Ctrl+C")) {}
            if (ImGui::MenuItem("Paste", "Ctrl+V")) {}
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Zoom in", "Ctrl+=")) {
                //scale = scale * 1.2f;
                //ImGui::GetIO().FontGlobalScale = scale;
            }
            if (ImGui::MenuItem("Zoom out", "Ctrl+-")) {
                //scale = scale * 0.8f;
                //ImGui::GetIO().FontGlobalScale = scale;
            }
            if (ImGui::MenuItem("Settings")) {}
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

CodeVisualizer::CodeVisualizer(std::string name, int& state, float& scale)
	: GrandWindow(name, state,  scale)
{
    docking_area = new DockingArea("Docking_Area");
    text_area = new TextArea("Text_Area");

    ImGuiID dockspace_id = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDocking);

    ImGuiID dock_id_left, dock_id_right;

    ImGui::DockBuilderRemoveNode(dockspace_id);
    ImGui::DockBuilderAddNode(dockspace_id);
    ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

    ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.7f, &dock_id_left, &dock_id_right);
    ImGui::DockBuilderDockWindow(docking_area->getName().c_str(), dock_id_left);
    ImGui::DockBuilderDockWindow(text_area->getName().c_str(), dock_id_right);

    ImGui::DockBuilderFinish(dockspace_id);
}

CodeVisualizer::~CodeVisualizer()
{
    delete docking_area;
    delete text_area;
}

void CodeVisualizer::update()
{
    updateMenuBar();

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDocking);
    docking_area->update();
    text_area->update();
}
