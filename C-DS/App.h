#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>

#define IMGUI_ENABLE_FREETYPE

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#include "DockingArea.h"
#include "TextArea.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>

class App
{
private:

	// private fields:

	GLFWwindow* window;
	ImGuiStyle* style;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	DockingArea* docking_area;
	TextArea* text_area;

	ImGuiIO* io;
	bool my_tool_active = false;
	float scale = 1;

	// private methods:
	void initWindow(int, int, std::string, bool);

public:

	App(int, int, std::string, bool);
	~App();
	

	// public methods:
	void run();
	void update();
	void render();

};

