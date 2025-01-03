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
    
    

    static int* comp;
    static unsigned char* frame_data;
    GLuint logoTextureID[2];
    int logo_width[2], logo_height[2];

    int buttons_cnt;
    ButtonData buttons[11] = { ButtonData("Grid", "Resources\\grids.png", 1),
                              ButtonData("Graph", "Resources\\graphs.png", 2),
                              ButtonData("Stack", "Resources\\stack.png", 10),
                              ButtonData("Queue", "Resources\\queue.png", 5),
                              ButtonData("Linked-List", "Resources\\linked_list.png", 7),
                              ButtonData("Deque", "Resources\\deque.png", 6),
                              ButtonData("Vector", "Resources\\vector.png", 11),
                              ButtonData("Hash Table", "Resources\\hash_table.png", 8), 
                              ButtonData("Hash Map", "Resources\\hash_map.png", 9),
                              ButtonData("DSU", "Resources\\DSU.png", 3),
                              ButtonData("Trie", "Resources\\trie.png", 12),
                              /*ButtonData("Sparse Table", "Resources\\sparce_table.png",30)*/ };

    // private methods:
    bool ImageButtonWithText(const char*, ImTextureID, const ImVec2&);
    void updateButtons();
    void drawLogo();

public:

    MainMenu(std::string, int&, float&, bool&, int&);
    ~MainMenu();

    // public methods:
    void update();

};

