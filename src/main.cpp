#include "Curve.hpp"
#include "CurveRenderer.hpp"
#include "Examples.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <cstdio>
#ifdef __linux__
    #include <unistd.h>
    #define GetCurrentDir getcwd
#else
    #include <direct.h>
    #define GetCurrentDir _getcwd
#endif
// include glm
#include <glm.hpp>

#define BERNSTEIN   1
#define CASTELJAU   2

// settings
unsigned int SCR_WIDTH = 1920;
unsigned int SCR_HEIGHT = 1080;
[[maybe_unused]] float lastX, lastY;
bool firstMouse = true;
double cursorXpos, cursorYpos;
float generationTime = 0.0f;

// render
void setUpRender(char curveFunction, Curve & curve, CurveRenderer & renderer);

// math
bool nearlyEqual(double a, double b, double epsilon);

// System
std::string getCurrentWorkingDirectory ();
void little_sleep(std::chrono::milliseconds us);

// Callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// GUI Staff
void displayMousePos(bool* p_open);
void renderGUI(char &curveFunction, int &precision, int &iteration,
               bool &regenerate, bool &hideConstruction, bool &showSteps,
               Curve &curve, CurveRenderer & crenderer);

void listenChanges(double & lastCursorX, double & lastCursorY, Curve & curve, bool & regenerate);
void listenToAddVertex(int & indexToModify, Curve &curve, bool & regenerate);
void CherryTheme() ;

// **************
// MAIN
int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow((int)SCR_WIDTH, (int)SCR_HEIGHT, "Bezier Curve Editor", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // setup callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // configure global opengl state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    // get current working directory
    std::string currentPath = getCurrentWorkingDirectory();
	std::cout << "Current working directory is " << currentPath << std::endl;

    // create curve
    Curve curve = Curve();

    // create a renderer for the curve
    CurveRenderer crenderer = CurveRenderer();
    crenderer.init();
    int indexToModify = -1;

    // create shaders
    Shader shaderCurve = Shader((currentPath+"assets/shaders/curve.vs.glsl").c_str(), (currentPath+"assets/shaders/curve.fs.glsl").c_str());
    Shader shaderEdition = Shader((currentPath+"assets/shaders/edition.vs.glsl").c_str(), (currentPath+"assets/shaders/edition.fs.glsl").c_str());
    Shader shaderConstruct = Shader((currentPath+"assets/shaders/construction.vs.glsl").c_str(), (currentPath+"assets/shaders/construction.fs.glsl").c_str());

    // setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    // setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");

    // setup Dear ImGui theme
    CherryTheme();
    ImGui::GetStyle().WindowMinSize = ImVec2((float)SCR_WIDTH*0.2f, (float)SCR_HEIGHT);
    ImGui::GetIO().FontGlobalScale = 1 + float(SCR_WIDTH)/(1920);
    ImGui::GetIO().Fonts->AddFontFromFileTTF("../assets/fonts/Roboto-Light.ttf", 16.0f);

    // ------------------
    bool overlay(true), hideConstruction(false), showSteps(false), regenerate(false);
    char curveFunction = CASTELJAU;
    double lastCursorX, lastCursorY;
    int precision(50), iteration(100);
    generationTime = 0;
    // RENDER LOOP -----
    bool m_first_loop(true);
    while (!glfwWindowShouldClose(window))
    {   
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        glClearColor(50.0f/255.0f, 50.0f/255.0f, 50.0f/255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        listenChanges(lastCursorX, lastCursorY, curve, regenerate);
        listenToAddVertex(indexToModify, curve, regenerate);

        // feed inputs to dear imgui, start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        displayMousePos(&overlay);

        if (m_first_loop){
            curve.clear();
            crenderer.clear();
            curve.clear();
            crenderer.clear();
            curve.setControlPoints(heartPoints);
            regenerate = true;
            m_first_loop = false;
        }
        // rendering our geometries
        crenderer.draw(shaderCurve, shaderEdition, shaderConstruct, hideConstruction, showSteps);
        // render your GUI
        renderGUI(curveFunction, precision, iteration, regenerate, hideConstruction, showSteps,
                  curve, crenderer);

        little_sleep(std::chrono::milliseconds(25));
    }
    // clean up
    crenderer.cleanUp();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glDeleteShader(shaderCurve.ID);
    glDeleteShader(shaderEdition.ID);
    glDeleteShader(shaderConstruct.ID);
    glfwDestroyWindow(window);
    return 0;
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    glViewport(0, 0, width, height);
    ImGui::GetIO().FontGlobalScale = 1 + float(SCR_WIDTH)/(1920);
    ImGui::GetStyle().WindowMinSize = ImVec2((float)SCR_WIDTH*0.2f, (float)SCR_HEIGHT);
}


// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = (float) xpos;
        lastY = (float) ypos;
        firstMouse = false;
    }

    lastX = (float) xpos;
    lastY = (float) ypos;
}


// glfw: mouse button callback
// ---------------------------
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) 
    {
       //getting cursor position
       glfwGetCursorPos(window, &cursorXpos, &cursorYpos);
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
}

// Those light colors are better suited with a thicker font than the default one + FrameBorder
// From https://github.com/procedural/gpulib/blob/master/gpulib_imgui.h
void CherryTheme() {
    // cherry colors, 3 intensities
#define HI(v)   ImVec4(0.35f, 0.35f, 0.35f, v)
#define MED(v)  ImVec4(0.20, 0.20, 0.20, v)
#define LOW(v)  ImVec4(0.25, 0.25, 0.25, v)
    // backgrounds (@todo: complete with BG_MED, BG_LOW)
#define BG(v)   ImVec4(0.15, 0.15f, 0.15f, v)
    // text
#define TEXT(v) ImVec4(0.860f, 0.930f, 0.890f, v)

    auto &style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text]                  = TEXT(0.78f);
    style.Colors[ImGuiCol_TextDisabled]          = TEXT(0.28f);
    style.Colors[ImGuiCol_WindowBg]              = BG( 0.9f);//ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_ChildBg]               = BG( 0.58f);
    style.Colors[ImGuiCol_PopupBg]               = BG( 0.9f);
    style.Colors[ImGuiCol_Border]                = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
    style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]               = BG( 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]        = MED( 0.78f);
    style.Colors[ImGuiCol_FrameBgActive]         = MED( 1.00f);
    style.Colors[ImGuiCol_TitleBg]               = LOW( 1.00f);
    style.Colors[ImGuiCol_TitleBgActive]         = HI( 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = BG( 0.75f);
    style.Colors[ImGuiCol_MenuBarBg]             = BG( 0.47f);
    style.Colors[ImGuiCol_ScrollbarBg]           = BG( 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = MED( 0.78f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = MED( 1.00f);
    style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_Button]                = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_ButtonHovered]         = MED( 0.86f);
    style.Colors[ImGuiCol_ButtonActive]          = MED( 1.00f);
    style.Colors[ImGuiCol_Header]                = MED( 0.76f);
    style.Colors[ImGuiCol_HeaderHovered]         = MED( 0.86f);
    style.Colors[ImGuiCol_HeaderActive]          = HI( 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
    style.Colors[ImGuiCol_ResizeGripHovered]     = MED( 0.78f);
    style.Colors[ImGuiCol_ResizeGripActive]      = MED( 1.00f);
    style.Colors[ImGuiCol_PlotLines]             = TEXT(0.63f);
    style.Colors[ImGuiCol_PlotLinesHovered]      = MED( 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]         = TEXT(0.63f);
    style.Colors[ImGuiCol_PlotHistogramHovered]  = MED( 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]        = MED( 0.43f);
    style.Colors[ImGuiCol_ModalWindowDarkening]  = BG( 0.73f);

    style.WindowPadding            = ImVec2(6, 4);
    style.WindowRounding           = 0.0f;
    style.FramePadding             = ImVec2(5, 2);
    style.FrameRounding            = 3.0f;
    style.ItemSpacing              = ImVec2(7, 1);
    style.ItemInnerSpacing         = ImVec2(1, 1);
    style.TouchExtraPadding        = ImVec2(0, 0);
    style.IndentSpacing            = 6.0f;
    style.ScrollbarSize            = 12.0f;
    style.ScrollbarRounding        = 16.0f;
    style.GrabMinSize              = 20.0f;
    style.GrabRounding             = 2.0f;

    style.WindowTitleAlign.x = 0.50f;

    style.Colors[ImGuiCol_Border] = ImVec4(0.539f, 0.479f, 0.255f, 0.162f);
    style.FrameBorderSize = 0.0f;
    style.WindowBorderSize = 1.0f;
}


void little_sleep(std::chrono::milliseconds us)
{
    auto start = std::chrono::high_resolution_clock::now();
    auto end = start + us;
    do {
        std::this_thread::yield();
        //if (!awake) std::cout << "sleep" << std::endl;
        //else {std::cout << "-------- AWAKE" << std::endl; break;}
    } while (std::chrono::high_resolution_clock::now() < end);
}

bool nearlyEqual(double a, double b, double epsilon)
{
    // if the distance between a and b is less than epsilon, then a and b are "close enough"
    return std::abs(a - b) <= epsilon;
}

void setUpRender(char curveFunction, Curve & curve, CurveRenderer & renderer){
    float lastTime;
    switch (curveFunction) {
        case BERNSTEIN:
            lastTime = (float) glfwGetTime();
            renderer.setUpVBO(curve.getControlPoints(), curve.bezierCurveByBernstein(curve.getControlPoints()), curve.getIteration(), curve.getPrecision());
            generationTime = (float) glfwGetTime() - lastTime;
            break;

        case CASTELJAU:
            lastTime = (float) glfwGetTime();
            renderer.setUpVBO(curve.getControlPoints(), curve.bezierCurveByCasteljau(curve.getControlPoints()), curve.getIteration(), curve.getPrecision());
            generationTime = (float) glfwGetTime() - lastTime;
            break;
        default:
            break;
    }
}

void displayMousePos(bool* p_open)
{
    const float DISTANCE = 10.0f;
    static int corner = 0;
    ImGuiIO& io = ImGui::GetIO();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    if (corner != -1)
    {
        window_flags |= ImGuiWindowFlags_NoMove;
        ImVec2 window_pos = ImVec2(io.DisplaySize.x - DISTANCE, DISTANCE);
        ImVec2 window_pos_pivot = ImVec2(1.0f, 0.0f);
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    }
    if (ImGui::Begin("Example: Simple overlay", p_open, window_flags| ImGuiWindowFlags_NoBackground))
    {
        double xpos, ypos;
        glfwGetCursorPos(glfwGetCurrentContext(), &xpos, &ypos);
        xpos = (xpos/SCR_WIDTH)*2.0 - 1.0;
        ypos = (-ypos/SCR_HEIGHT)*2.0 + 1.0;
        ImGui::Text("Mouse Position: (%.2f,%.2f)", xpos, ypos);
    }
    ImGui::End();
}



std::string getCurrentWorkingDirectory (){
    char cCurrentPath[FILENAME_MAX];
    if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath))){return std::string();}
    cCurrentPath[sizeof(cCurrentPath) - 1] = '\0';
    std::string currentPath = std::string(cCurrentPath);
    int found = currentPath.find("build");
    currentPath = currentPath.substr(0, found);
    unsigned int i = 0;
    while (i < currentPath.size()){
        if (currentPath[i] == '\\') {currentPath[i] = '/';}
        ++i;
    }
    return currentPath;
}


void renderGUI(char &curveFunction, int &precision, int &iteration,
               bool &regenerate, bool &hideConstruction, bool &showSteps,
               Curve &curve, CurveRenderer & crenderer){
    ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_MenuBar);
    {
        ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Once);
        ImGui::SetWindowSize(ImVec2(400, (float)SCR_HEIGHT));
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        float rightSpacing = 5.0f;
        ImGui::Dummy(ImVec2(rightSpacing, 0.0f));
        ImGui::SameLine();
        if(ImGui::RadioButton("Bernstein", curveFunction == BERNSTEIN)) {
            curveFunction = BERNSTEIN;
            regenerate = true;
        }
        ImGui::Dummy(ImVec2(rightSpacing, 0.0f));
        ImGui::SameLine();
        if(ImGui::RadioButton("Casteljau", curveFunction == CASTELJAU)) {
            curveFunction = CASTELJAU;
            regenerate = true;
        }
        ImGui::Dummy(ImVec2(0.0f, 20.0f));
        ImGui::Text("%s", ("Create in "+std::to_string(generationTime)+" seconds").c_str());
        ImGui::Dummy(ImVec2(0.0f, 20.0f));
        ImGui::Separator();
        ImGui::SliderInt(" Precision", &precision, 1, 150);
        if (precision != curve.getPrecision()) {
            curve.setPrecision((float)precision);
            if(!curve.getControlPoints().empty()) regenerate = true;
        }
        ImGui::SliderInt(" %", &iteration, 1, 100);
        float perit = (float)precision * ((float)iteration/100.0f);
        if (perit != (float)curve.getIteration()) {
            curve.setPrecision((float)precision);
            curve.setIteration((int)perit);
            if(!curve.getControlPoints().empty()) regenerate = true;
        }

        ImGui::Checkbox(" Hide Control Points", &hideConstruction);
        ImGui::Checkbox(" Show steps lines", &showSteps);
        ImGui::Dummy(ImVec2(0.0f, 20.0f));

        ImGui::SetNextTreeNodeOpen(true);
        if(ImGui::CollapsingHeader("Control Points"))
        {
            for (unsigned int i = 0; i < curve.getControlPoints().size(); ++i) {
                std::string str = "Vertex "+ std::to_string(i);
                glm::vec3 controlP = curve.getControlPoints()[i];
                ImGui::InputFloat2(str.c_str(), &controlP.x,2);

                if (controlP.x != curve.getControlPoints()[i].x || controlP.y != curve.getControlPoints()[i].y) regenerate = true;
                curve.getControlPoints()[i].x = controlP.x;
                curve.getControlPoints()[i].y = controlP.y;
            }
        }



        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Load")) {
                std::string str = (" "+std::to_string(circlePoints.size())+" vertices");
                if (ImGui::MenuItem("Circle ", str.c_str())){
                    curve.clear();
                    crenderer.clear();
                    curve.setControlPoints(circlePoints);
                    regenerate = true;
                }
                str = (" "+std::to_string(eightPoints.size())+" vertices");
                if (ImGui::MenuItem("Eight ", str.c_str())){
                    curve.clear();
                    crenderer.clear();
                    curve.setControlPoints(eightPoints);
                    regenerate = true;
                }
                str = (" "+std::to_string(heartPoints.size())+" vertices");
                if (ImGui::MenuItem("Heart ", str.c_str())){
                    curve.clear();
                    crenderer.clear();
                    curve.setControlPoints(heartPoints);
                    regenerate = true;
                }
                str = (" "+std::to_string(moonPoints.size())+" vertices");
                if (ImGui::MenuItem("Moon ", str.c_str())){
                    curve.clear();
                    crenderer.clear();
                    curve.setControlPoints(moonPoints);
                    regenerate = true;
                }
                str = (" "+std::to_string(wavePoints.size())+" vertices");
                if (ImGui::MenuItem("Wave ", str.c_str())){
                    curve.clear();
                    crenderer.clear();
                    curve.setControlPoints(wavePoints);
                    regenerate = true;
                }
                ImGui::EndMenu();
            }
            ImGui::Dummy(ImVec2(5.0f, 0.0f));
            if (ImGui::BeginMenu("Clear")) {
                curve.clear();
                crenderer.clear();
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }
    ImGui::End();

    if (regenerate && !curve.getControlPoints().empty()) {
        setUpRender(curveFunction, curve, crenderer);
        regenerate = false;
    }

    // Render dear imgui into screen
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void listenChanges(double & lastCursorX, double & lastCursorY, Curve & curve, bool & regenerate){
    if (lastCursorX != cursorXpos || lastCursorY != cursorYpos)
    {
        curve.getControlPoints().emplace_back(glm::vec3((cursorXpos/SCR_WIDTH)*2.0 - 1.0, (-cursorYpos/SCR_HEIGHT)*2.0 + 1.0, 0));
        lastCursorX = cursorXpos;
        lastCursorY = cursorYpos;

        //glm::vec3 tmp = curve.getControlPoints().at(curve.getControlPoints().size()-1);
        //std::cout << tmp.x << " " << tmp.y << std::endl;

        regenerate = true;
    }
}

void listenToAddVertex(int & indexToModify, Curve &curve, bool & regenerate){
    if (glfwGetMouseButton(glfwGetCurrentContext(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
        double xpos, ypos;
        glfwGetCursorPos(glfwGetCurrentContext(), &xpos, &ypos);
        xpos = (xpos/SCR_WIDTH)*2.0 - 1.0;
        ypos = (-ypos/SCR_HEIGHT)*2.0 + 1.0;

        for (unsigned int i = 0 ; i < curve.getControlPoints().size() ; ++i)
        {
            if (nearlyEqual(curve.getControlPoints().at(i).x, xpos, 0.05) && nearlyEqual(curve.getControlPoints().at(i).y, ypos, 0.05)){
                indexToModify = (int) i;
            }
        }

        if (indexToModify != -1) {
            curve.getControlPoints().at(indexToModify) = glm::vec3(xpos, ypos, 0);
            regenerate = true;
        }
    }
    if (glfwGetMouseButton(glfwGetCurrentContext(), GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS) indexToModify = -1;
}
