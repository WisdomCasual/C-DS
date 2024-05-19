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
        int imageWidth = 0, imageHeight = 0, stateID = 0;
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
    ButtonData buttons[13] = { ButtonData("Grid", "Resources\\grids.png", 1),
                              ButtonData("Graph", "Resources\\graphs.png", 2),
                              ButtonData("DSU", "Resources\\DSU.png", 3),
                              ButtonData("Trees", "Resources\\mob.png", 4),
                              ButtonData("Trie", "Resources\\mob.png", 12),
                              ButtonData("Queue", "Resources\\mob.png", 5),
                              ButtonData("Deque", "Resources\\mob.png", 6),
                              ButtonData("Linked-List", "Resources\\mob.png", 7),
                              ButtonData("Hash Table", "Resources\\mob.png", 8), 
                              ButtonData("Hash Map", "Resources\\mob.png", 9),
                              ButtonData("Stack", "Resources\\mob.png", 10),
                              ButtonData("Vector", "Resources\\mob.png", 11),
                              ButtonData("Sparse Table", "Resources\\mob.png",30) };

    // private methods:
    bool ImageButtonWithText(const char*, ImTextureID, const ImVec2&);
    void updateButtons();

public:

    MainMenu(std::string, int&, float&, bool&);
    ~MainMenu();

    // public methods:
    void update();

};

