#include "MainMenu.h"
#include <imgui_internal.h>
#include "Image.h"

#include <iostream>

bool MainMenu::ImageButtonWithText(const char* label, ImTextureID texId, const ImVec2& imageSize) {

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImGuiIO* io = &ImGui::GetIO();
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    if (window->SkipItems)
        return false;

    ImVec2 size = imageSize;
    size.x -= 2.f * style.FramePadding.x;
    size.y -= 2.f * style.FramePadding.y;

    if (size.x <= 0 && size.y <= 0) { size.x = size.y = ImGui::GetTextLineHeightWithSpacing(); }
    else {
        if (size.x <= 0)          size.x = size.y;
        else if (size.y <= 0)     size.y = size.x;

        size.x *= window->FontWindowScale * ImGui::GetIO().FontGlobalScale;
        size.y *= window->FontWindowScale * ImGui::GetIO().FontGlobalScale;
    }


    const ImGuiID id = window->GetID(label);
    const ImVec2 textSize = ImGui::CalcTextSize(label, NULL, true);
    const bool hasText = textSize.x > 0;

    const float innerSpacing = hasText ? style.ItemInnerSpacing.x : 0.f;
    const ImVec2 padding = style.FramePadding;
    const ImVec2 totalSizeWithoutPadding(size.x > textSize.x ? size.x : textSize.x, size.y + innerSpacing + textSize.y);

    const ImRect bb(window->DC.CursorPos, ImVec2(window->DC.CursorPos.x + totalSizeWithoutPadding.x + padding.x * 2, window->DC.CursorPos.y + totalSizeWithoutPadding.y + padding.y * 2));


    ImVec2 start(0, 0);
    start = ImVec2(window->DC.CursorPos.x + padding.x, window->DC.CursorPos.y + padding.y); if (size.x < textSize.x) start.x += (textSize.x - size.x) * .5f;
    const ImRect image_bb(start, ImVec2(start.x + size.x, start.y + size.y));

    start = ImVec2(window->DC.CursorPos.x + padding.x, window->DC.CursorPos.y + padding.y); if (size.x > textSize.x) start.x += (size.x - textSize.x) * .5f; start.y += size.y + innerSpacing;

    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, id))
        return false;

    bool hovered = false, held = false;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

    // Render
    const ImU32 col = ImGui::GetColorU32((hovered && held) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
    ImGui::RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);

    window->DrawList->AddImageRounded(texId, image_bb.Min, image_bb.Max, ImVec2(0, 0), ImVec2(1, 1), ImGui::GetColorU32(ImVec4(1, 1, 1, 1)), style.FrameRounding);

    if (textSize.x > 0) ImGui::RenderText(start, label);

    return pressed;
}

MainMenu::MainMenu(std::string name, int& state, float& GuiScale, bool& settingsEnabled, int& colorMode)
	: GrandWindow(name, state, GuiScale, settingsEnabled, colorMode)
{

    buttons_cnt = (sizeof buttons) / (sizeof(buttons[0]));

    for (auto& cur_button : buttons) {
        if (!image_imp::loadImage(cur_button.imagePath.c_str(), &cur_button.textureID, &cur_button.imageWidth, &cur_button.imageHeight)) {
            std::cout << "failed to load image\n";
        }
    }

    image_imp::loadImage("Resources\\C-DS.png", &logoTextureID, &logoWidth, &logoHeight);

}

MainMenu::~MainMenu()
{

}

void MainMenu::updateButtons()
{
    ImVec2 item_spacing = ImGui::GetStyle().ItemSpacing;
    float usabe_area = viewport->WorkSize.x * 0.8f;
    int button_idx = 0, per_row = std::max((int)floor(viewport->WorkSize.x / (BUTTON_WIDTH * GuiScale + item_spacing.x)), 1);


    float button_width = (usabe_area / (float)per_row - item_spacing.x) / GuiScale, text_height = 45.f * GuiScale;

    if (per_row > buttons_cnt) {
        button_width = BUTTON_WIDTH;
    }

    float button_height = button_width * 3.f / 4.f;

    int row_idx = 1;

    ImVec2 pos(viewport->WorkPos.x + (viewport->WorkSize.x - usabe_area) / 2.f, ImGui::GetCursorPosY());

    if (per_row > buttons_cnt)
        ImGui::SetCursorPosX(viewport->WorkPos.x + (viewport->WorkSize.x - ((button_width * GuiScale + item_spacing.x) * buttons_cnt)) / 2.f);
    else 
        ImGui::SetCursorPosX(pos.x);


    if (((buttons_cnt + per_row - 1) / per_row) * (button_height * GuiScale + text_height + item_spacing.y) + 50.f * GuiScale < viewport->WorkSize.y - pos.y) {
        pos.y += (viewport->WorkSize.y - pos.y - ((buttons_cnt + per_row - 1) / per_row) * (button_height * GuiScale + text_height + item_spacing.y)) / 2.f;
    }

    ImGui::SetCursorPosY(pos.y);


    while (button_idx < buttons_cnt) {
        if (ImageButtonWithText(buttons[button_idx].label.c_str(), (void*)(intptr_t)buttons[button_idx].textureID, ImVec2(button_width * 1.5f, button_height * 1.5f))) {
            state = buttons[button_idx].stateID;
        }
        if (row_idx == per_row) {
            row_idx = 1;
            pos.y += button_height * GuiScale + text_height + item_spacing.y;

            int rem = buttons_cnt - button_idx - 1;
            if (rem < per_row)
                ImGui::SetCursorPosX(viewport->WorkPos.x + (viewport->WorkSize.x - ((button_width * GuiScale + item_spacing.x) * rem)) / 2.f);
            else
                ImGui::SetCursorPosX(pos.x);

            ImGui::SetCursorPosY(pos.y);
        }
        else {
            ImGui::SameLine();
            row_idx++;
        }
        button_idx++;
    }
}

void MainMenu::update()
{

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);

	ImGui::Begin(getName().c_str(), NULL, flags);

    
    float logoScale = GuiScale;

    float logo_height = 300.f * GuiScale;
    if (logo_height > viewport->WorkSize.y * 2.f / 5.f) {
        logoScale = std::max(viewport->WorkSize.y * 2.f / 5.f, 100.f) / logo_height;
        logo_height = std::max(viewport->WorkSize.y * 2.f / 5.f, 100.f);
    }

    ImVec2 logo_size(logo_height * logoWidth / logoHeight, logo_height);

    if (logo_size.x > (viewport->WorkSize.x - ImGui::GetStyle().ScrollbarSize)) {
        logoScale = ((viewport->WorkSize.x - ImGui::GetStyle().ScrollbarSize) * logoHeight / logoWidth) / (300.f * GuiScale);
        logo_size = ImVec2((viewport->WorkSize.x - ImGui::GetStyle().ScrollbarSize), (viewport->WorkSize.x - ImGui::GetStyle().ScrollbarSize) * logoHeight / logoWidth);
    }

    ImGui::SetCursorPosX(viewport->WorkPos.x + viewport->WorkSize.x - 130.f * GuiScale);

    if (ImGui::Button("Settings")) {
        settingsEnabled = true;
    }

    ImGui::SetCursorPosX(viewport->WorkPos.x + (viewport->WorkSize.x - logo_size.x) / 2.f);
    ImGui::Image((void*)(intptr_t)logoTextureID, logo_size);

    updateButtons();

	ImGui::End();
	ImGui::PopStyleVar();

}
