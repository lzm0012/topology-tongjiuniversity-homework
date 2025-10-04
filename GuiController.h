#pragma once
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

struct GuiParams {
    int nodeCount = 20;
    float edgeProbability = 0.3f;
    bool is3D = true;
    int graphType = 0;
    int gridRows = 5;
    int gridCols = 5;

    float layoutStrength = 0.1f;
    float repulsionStrength = 100.0f;
    float attractionStrength = 0.1f;

    bool autoLayout = true;
    bool showNodes = true;
    bool showEdges = true;
};

class GuiController {
public:
    GuiParams params;
    void initialize(GLFWwindow* window);
    void render();
    void shutdown();
    bool shouldRegenerate() const { return regenerate; }
    void resetRegenerateFlag() { regenerate = false; }

    bool shouldExportSVG() const { return exportSVG; }
    void resetExportFlag() { exportSVG = false; }

private:
    bool regenerate = false;
    bool exportSVG = false;
};
