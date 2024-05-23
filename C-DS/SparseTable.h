#pragma once
#include "GrandWindow.h"
#include <imgui.h>
#include <vector>
#include <string>
class SparseTable : public GrandWindow
{
private:
    // grid size constraints:
    #define X_MAX 50
    #define X_MIN 2
    #define Y_MAX 50
    #define Y_MIN 2

    // speed constraints:
    #define GRID_MAX_SPEED 100.0f
    #define GRID_MIN_SPEED 0.1f
    #define GRID_DELAY_TIME 0.2f

    #define CELL_SIZE 70.f
    #define SEPARATOR_SIZE 5.f

    // private fields:
    ImGuiWindowFlags main_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGuiWindowFlags controls_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuiIO* io;

    int cur_tool = 0, LOG = 18, x_size, y_size,sz=0;
    float speed = 1.f, curTime = 0;
    bool cleared = true, paused = false, camFollow = false, movingCam = false, building = false, built=false;
    ImVec2 camPos = { 0, 0 }, camTarget = { 0, 0 }, cur_pos{ 0,0 }, s_pos{ 0,0 }, cur_cell{ 0,0 };
    char buffer[256];
    std::vector<int>Elements;
    std::vector<std::vector<bool>>selected;
    std::vector<std::vector<int>>table;
    std::vector<int>logs;
    /*
        Sparse_Table[LOG][arr_size]
        first row will hold the full array 
        for every row will hold the answer for the query that starts at indx i and ends at j + 2^i
        first the user will enter the array and 
        and the grid will adjust to fit the number
        then he will press the build button
        when building every cell will be highlighted and the cells it will take it's answer from

    */

    void controlsUpdate();
    ImU32 getColor(int, int);
    void build();
    void init();
    void drawtable();
    void drawarray(float);
public:
    void update();
    SparseTable(std::string, int&, float&, bool&);
    ~SparseTable();
};

