#include "TextArea.h"

TextArea::TextArea(std::string name)
    : name(name)
{
    window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDocking;
}

TextArea::~TextArea()
{

}

void TextArea::update()
{
    ImGui::SetNextWindowClass(&window_class);
    ImGui::Begin(name.c_str(), NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

    if (ImGui::Button("Run")) {

    }

    ImGui::SameLine();
    ImGui::Text("Text 2");
    static char text[int(1e4)];
    ImGui::InputTextMultiline("IDE", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, -FLT_MIN));

    ImGui::End();
}

const std::string TextArea::getName()
{
    return name;
}
