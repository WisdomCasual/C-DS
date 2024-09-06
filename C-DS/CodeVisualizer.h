#pragma once
#include "GrandWindow.h"
#include <imgui.h>
#include <imgui_internal.h>

#include "DockingArea.h"
#include "TextArea.h"

class CodeVisualizer :
    public GrandWindow
{
private:
    // private fields:
    DockingArea* docking_area;
    TextArea* text_area;

    // private methods:
    void updateMenuBar();

public:

    CodeVisualizer(std::string, int&, float&, bool&, int&);
    ~CodeVisualizer();

    // public methods:
    void update();

};

