#include "MainMenu.h"


void MainMenu::updateMenuBar()
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
                scale = scale * 1.2f;
                ImGui::GetIO().FontGlobalScale = scale;
            }
            if (ImGui::MenuItem("Zoom out", "Ctrl+-")) {
                scale = scale * 0.8f;
                ImGui::GetIO().FontGlobalScale = scale;
            }
            if (ImGui::MenuItem("Settings")) {
                settingsEnabled = true;
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

MainMenu::MainMenu(std::string name, int& state, float& scale, bool& settingsEnabled)
	: GrandWindow(name, state, scale, settingsEnabled)
{

}

MainMenu::~MainMenu()
{

}

void MainMenu::update()
{
    updateMenuBar();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);

	ImGui::Begin(getName().c_str(), NULL, flags);

    if (ImGui::Button("Grid"))
        state = 1;
    
    if (ImGui::Button("Graph"))
        state = 2;

    if (ImGui::Button("Code Visualier"))
        state = 10;



	ImGui::End();
	ImGui::PopStyleVar();
}
