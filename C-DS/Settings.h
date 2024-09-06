#pragma once
#include "GrandWindow.h"
#include <imgui.h>

class Settings :
    public GrandWindow
{
    // private fields:
    ImGuiStyle* style;
    bool initialized = false, debuggingSettings = false;

public:

    Settings(std::string, int&, float&, bool&, int&);
    ~Settings();

    // public methods:
    void update();

};

