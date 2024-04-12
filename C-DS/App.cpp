#include "App.h"
#include <imgui_internal.h>

void App::initWindow(int x, int y, std::string title, bool vSyncEnabled)
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
    window = glfwCreateWindow(x, y, title.c_str(), nullptr, nullptr);
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

    // Setup Dear ImGui style
    //if(colorMode == 0)
    //    ImGui::StyleColorsDark();
    //else
    //    ImGui::StyleColorsLight();

    // load custom font
    ImFont* customFont = io->Fonts->AddFontFromFileTTF("Resources/font.ttf", 20.0f);

    // Set custom font as default
    io->FontDefault = customFont;

    // Set window rounding radius
    style->WindowRounding = 10.0f;

    style->WindowPadding = ImVec2(14, 14);
    style->ItemSpacing = ImVec2(10, 8);


}

void App::updateWindow()
{
    if (curWindow != nullptr) {
        delete curWindow;
        curWindow = nullptr;
    }

    switch (state) {
    case 0:
        {
            curWindow = new MainMenu("Main_Menu", state, scale, settingsEnabled);
            break;
        }
    case 1:
        {
            curWindow = new Grid("Grid", state, scale, settingsEnabled);
            break;
        }
    case 10:
        {
            curWindow = new CodeVisualizer("Code_Visualizer", state, scale, settingsEnabled);
            break;
        }
    }

    state = -1;

}

App::App(int x, int y, std::string title, bool vSyncEnabled)
{
    initWindow(x, y, title, vSyncEnabled);
    settings = new Settings("settings", state, scale, settingsEnabled, colorMode);
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
