#include "GuiController.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void GuiController::initialize(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void GuiController::render() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Topology Graph Parameter Control");

    ImGui::Text("Graph Generation Parameters:");


    if (params.graphType == 1) {
        ImGui::SliderInt("Rows", &params.gridRows, 2, 20);
        ImGui::SliderInt("Columns", &params.gridCols, 2, 20);
        params.nodeCount = params.gridRows * params.gridCols;
        ImGui::Text("Total Nodes: %d", params.nodeCount);
    }
    else {
        ImGui::SliderInt("Node Count", &params.nodeCount, 5, 500);
    }

    if (params.graphType != 1) {
        ImGui::SliderFloat("Edge Probability", &params.edgeProbability, 0.0f, 1.0f);
    }

    ImGui::Separator();

    ImGui::Text("Graph Type:");
    ImGui::RadioButton("Random Graph", &params.graphType, 0);
    ImGui::RadioButton("Grid Graph", &params.graphType, 1);
    ImGui::RadioButton("Circular Graph", &params.graphType, 2);
    ImGui::RadioButton("Star Graph", &params.graphType, 3);

    ImGui::Separator();

    ImGui::Checkbox("3D Mode", &params.is3D);

    ImGui::Separator();

    ImGui::Text("Layout Parameters:");
    ImGui::SliderFloat("Layout Strength", &params.layoutStrength, 0.01f, 1.0f);
    ImGui::SliderFloat("Repulsion Strength", &params.repulsionStrength, 1.0f, 1000.0f);
    ImGui::SliderFloat("Attraction Strength", &params.attractionStrength, 0.01f, 1.0f);

    ImGui::Separator();

    ImGui::Checkbox("Auto Layout", &params.autoLayout);
    ImGui::Checkbox("Show Nodes", &params.showNodes);
    ImGui::Checkbox("Show Edges", &params.showEdges);

    if (ImGui::Button("Regenerate")) {
        regenerate = true;
    }

    ImGui::End();

    ImGui::SameLine();
    if (ImGui::Button("Export SVG")) {
        exportSVG = true;
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GuiController::shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
