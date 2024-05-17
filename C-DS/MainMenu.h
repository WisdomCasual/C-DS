#pragma once
#include "imgui.h"
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>

#include "GrandWindow.h"
#include <vector>

class MainMenu :
    public GrandWindow
{
private:

    #define BUTTON_WIDTH 500 
    #define BUTTON_HEIGHT 375 

    struct ButtonData {
        const std::string label, imagePath;
        GLuint textureID;
        int imageWidth, imageHeight, stateID;
        ButtonData(std::string label, std::string imagePath, int stateID)
            : label(label), imagePath(imagePath), stateID(stateID) { textureID = NULL; }
    };


    // private fields:
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    

    static int* comp;
    static unsigned char* frame_data;
    GLuint logoTextureID;
    int logoWidth, logoHeight;

    int buttons_cnt;
    ButtonData buttons[5] = { ButtonData("Grid", "Resources\\grids.png", 1),
                               ButtonData("Graph", "Resources\\graphs.png", 2),
                               ButtonData("DSU", "Resources\\DSU.png", 3),
                               ButtonData("Trees", "Resources\\mob.png", 4),
                               ButtonData("Queue", "Resources\\mob.png", 5) };

    // private methods:
    bool ImageButtonWithText(const char*, ImTextureID, const ImVec2&);
    void updateButtons();

public:

    MainMenu(std::string, int&, float&, bool&);
    ~MainMenu();

    // public methods:
    void update();

};

