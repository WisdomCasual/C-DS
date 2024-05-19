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

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Settings.h"
#include "MainMenu.h"
#include "Grid.h"
#include "GraphTools.h"
#include "DSU.h"
#include "CodeVisualizer.h"
#include "Tree.h"
#include "Trie.h"
#include "QueueVisualization.h"
#include "SparseTable.h"
#include "LinkedList.h"
#include "Deque.h"
#include "HashTable.h"
#include "HashMap.h"
#include "Stack.h"

class App
{
private:

	// private fields:
	GLFWwindow* window;
	ImGuiStyle* style;
	const ImVec4 clear_color = ImVec4(0.f, 0.f, 0.f, 1.00f);

	GrandWindow* curWindow = nullptr;
	Settings* settings = nullptr;

	ImGuiIO* io;
	bool vSyncEnabled = true;
	int windowPosX = 100, windowPosY = 100;
	int windowWidth = 1280, windowHeight = 720;
	bool settingsEnabled = false, overlayEndabled = false, f3Pressed = false;
	bool isFullscreen = false, f11Pressed = false;
	int colorMode = 0;
	int state = 0;
	float GuiScale = 1.f;
	bool zoomIn = false, zoomOut = false;

	// private methods:
	void initWindow(std::string);
	void initStyle();
	void toggleFullscreen();
	void updateWindow();
	void overlay();

public:

	App(std::string);
	~App();
	

	// public methods:
	void run();
	void update();
	void render();
	GLFWwindow* getWindow();

};

