#include "App.h"
#include "Image.h"
#include <iostream>


void App::initWindow(std::string title)
{

    // Decide GL+GLSL versions
    #if defined(IMGUI_IMPL_OPENGL_ES2)
        // GL ES 2.0 + GLSL 100
        const char* glsl_version = "#version 100";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    #elif defined(__APPLE__)
        // GL 3.2 + GLSL 150
        const char* glsl_version = "#version 150";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
    #else
        // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
    #endif

    // Create window with graphics context
    window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), nullptr, nullptr);
    if (window == nullptr)
        exit(1);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(vSyncEnabled); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO(); (void)io;
   
    initStyle();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);

    ImGui_ImplOpenGL3_Init(glsl_version);

    //initialize window icon

    image_imp::SetWindowIcon(window, "Resources\\icon.png");

}

void App::initStyle()
{
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io->ConfigWindowsMoveFromTitleBarOnly = true;              // Move Windows from the title bar only

    //io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     // Enable Multi-Viewport / Platform Windows
    //io->ConfigViewportsNoAutoMerge = true;
    //io->ConfigViewportsNoTaskBarIcon = true;


    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    style = &ImGui::GetStyle();

    if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style->WindowRounding = 0.0f;
        style->Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    //Setup Dear ImGui style
    if(colorMode == 0)
        ImGui::StyleColorsDark();
    else
        ImGui::StyleColorsLight();

    // load custom font
    ImFont* customFont = io->Fonts->AddFontFromFileTTF("Resources/font.ttf", 45.0f);
    ImGui::GetIO().FontGlobalScale = GuiScale / 1.5f;

    // Set custom font as default
    io->FontDefault = customFont;

    // Set window rounding radius
    style->WindowRounding = 10.0f;


    style->WindowPadding = ImVec2(14, 14);
    style->ItemSpacing = ImVec2(10, 8);

    style->GrabRounding = style->FrameRounding = 8.f;

}

void App::toggleFullscreen()
{
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    if (isFullscreen)
        glfwSetWindowMonitor(window, nullptr, windowPosX, windowPosY, windowWidth, windowHeight, GLFW_DONT_CARE);
    else {
        glfwGetWindowPos(window, &windowPosX, &windowPosY);
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
    }
    glfwSwapInterval(vSyncEnabled);

    isFullscreen = !isFullscreen;
}

void App::updateWindow()
{
    if (curWindow != nullptr) {
        delete curWindow;
        curWindow = nullptr;
    }

    switch (state) {

    case 0:
        curWindow = new MainMenu("Main_Menu", state, GuiScale, settingsEnabled, colorMode);
        break;
    case 1:
        curWindow = new Grid("Grid", state, GuiScale, settingsEnabled, colorMode);
        break;
    case 2:
        curWindow = new GraphTools("Graph_Tools", state, GuiScale, settingsEnabled, colorMode);
        break;
    case 3:
        curWindow = new DSU("DESU?", state, GuiScale, settingsEnabled, colorMode);
        break;
    case 4:
        curWindow = new Tree("Tree", state, GuiScale, settingsEnabled, colorMode);
        break;
    case 5:
        curWindow = new QueueVisualization("Queue", state, GuiScale, settingsEnabled, colorMode);
        break;
    case 6:
        curWindow = new Deque("Deque", state, GuiScale, settingsEnabled, colorMode);
        break;
    case 7:
        curWindow = new LinkedList("Linked_List", state, GuiScale, settingsEnabled, colorMode);
        break;
    case 8:
        curWindow = new HashTable("Hash_Table", state, GuiScale, settingsEnabled, colorMode);
        break;
    case 9:
        curWindow = new HashMap("Hash_Map", state, GuiScale, settingsEnabled, colorMode);
        break;
    case 10:
        curWindow = new Stack("Stack", state, GuiScale, settingsEnabled, colorMode);
        break;
    case 11:
        curWindow = new Vector("Vector", state, GuiScale, settingsEnabled, colorMode);
        break;
    case 12:
        curWindow = new Trie("Trie", state, GuiScale, settingsEnabled, colorMode);
        break;
    case 30:
        curWindow = new SparseTable("Sparsy", state, GuiScale, settingsEnabled, colorMode);
        break;
}
    state = -1;

}

void App::overlay()
{
    static int location = 1;
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    if (location >= 0)
    {
        const float PAD = 10.0f;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos;
        ImVec2 work_size = viewport->WorkSize;
        ImVec2 window_pos, window_pos_pivot;
        window_pos.x = (location & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
        window_pos.y = (location & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
        window_pos_pivot.x = (location & 1) ? 1.0f : 0.0f;
        window_pos_pivot.y = (location & 2) ? 1.0f : 0.0f;
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        ImGui::SetNextWindowViewport(viewport->ID);
        window_flags |= ImGuiWindowFlags_NoMove;
    }
    else if (location == -2)
    {
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        window_flags |= ImGuiWindowFlags_NoMove;
    }

    ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background

    if (ImGui::Begin("Debugging overlay", &overlayEndabled, window_flags))
    {
        ImGui::Text("Debugging overlay\n" "(right-click to change position)");
        ImGui::Separator();

        ImGui::Text("Frames/Second: %.f", io->Framerate);

        if (ImGui::IsMousePosValid())
            ImGui::Text("Mouse Position: (%.f,%.f)", io->MousePos.x, io->MousePos.y);
        else
            ImGui::Text("Mouse Position: <invalid>");

        ImGui::Text(isFullscreen ? "Window Mode: Fullscreen" : "Window Mode: Windowed");

        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("Custom", NULL, location == -1)) location = -1;
            if (ImGui::MenuItem("Center", NULL, location == -2)) location = -2;
            if (ImGui::MenuItem("Top-left", NULL, location == 0)) location = 0;
            if (ImGui::MenuItem("Top-right", NULL, location == 1)) location = 1;
            if (ImGui::MenuItem("Bottom-left", NULL, location == 2)) location = 2;
            if (ImGui::MenuItem("Bottom-right", NULL, location == 3)) location = 3;
            if (overlayEndabled && ImGui::MenuItem("Close")) overlayEndabled = false;
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}

App::App(std::string title)
{
    initWindow(title);
    settings = new Settings("settings", state, GuiScale, settingsEnabled, colorMode);
}

App::~App()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    if (curWindow != nullptr)
        delete curWindow;

    if (settings != nullptr)
        delete settings;
}

void App::run()
{

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        update();
        render();
    }

}

void App::update()
{

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (state != -1)
        updateWindow();

    curWindow->update();

    if (settingsEnabled)
        settings->update();

    if(overlayEndabled)
        overlay();

    if (io->KeysDown[ImGuiKey_F3]) {
        if(!f3Pressed)
            overlayEndabled = !overlayEndabled;
        f3Pressed = true;
    }
    else {
        f3Pressed = false;
    }

    if (io->KeysDown[ImGuiKey_F11] || (isFullscreen && io->KeysDown[ImGuiKey_Escape])) {
        if (!f11Pressed)
            toggleFullscreen();
        f11Pressed = true;
    }
    else {
        f11Pressed = false;
    }

    if (io->KeyCtrl && io->KeysDown[ImGuiKey_Minus]) {
        if (!zoomOut) {
            zoomOut = true;
            GuiScale -= 0.2f;
            GuiScale = std::max(GuiScale, 0.6f);
            ImGui::GetIO().FontGlobalScale = GuiScale / 1.5f;
        }
    }
    else
        zoomOut = false;

    if (io->KeyCtrl && io->KeysDown[ImGuiKey_Equal]) {
        if (!zoomIn) {
            zoomIn = true;
            GuiScale += 0.2f;
            GuiScale = std::min(GuiScale, 2.f);
            ImGui::GetIO().FontGlobalScale = GuiScale / 1.5f;
        }
    }
    else
        zoomIn = false;
}

void App::render()
{
    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
    if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }

    glfwSwapBuffers(window);
}

GLFWwindow* App::getWindow()
{
    return window;
}
