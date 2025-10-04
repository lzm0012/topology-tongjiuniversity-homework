#define _USE_MATH_DEFINES
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <ctime>
#include "Graph.h"
#include "Camera.h"
#include "Renderer.h"
#include "GuiController.h"

GLFWwindow* window = nullptr;
Camera camera;
Renderer renderer;
Graph graph;
GuiController gui;
bool firstMouse = true;
float lastX = 400.0f, lastY = 300.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void adjustCameraToFitGraph() {
    if (graph.nodes.empty()) return;

    glm::vec3 min_pos = graph.nodes[0].position;
    glm::vec3 max_pos = graph.nodes[0].position;

    for (const auto& node : graph.nodes) {
        min_pos = glm::min(min_pos, node.position);
        max_pos = glm::max(max_pos, node.position);
    }

    glm::vec3 center = (min_pos + max_pos) * 0.5f;
    glm::vec3 size = max_pos - min_pos;
    float max_size = glm::max(glm::max(size.x, size.y), size.z);

    camera.setTarget(center);

    camera.distance = max_size * 2.0f;
    if (camera.distance < 5.0f) camera.distance = 10.0f;
    if (camera.distance > 50.0f) camera.distance = 50.0f;

    camera.position = center - camera.front * camera.distance;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

int main() {
    if (!glfwInit()) {
        std::cout << u8"GLFW初始化失败!" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(1200, 800, u8"拓扑图生成器", NULL, NULL);
    if (window == NULL) {
        std::cout << u8"窗口创建失败!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << u8"GLAD初始化失败!" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    renderer.initialize();
    gui.initialize(window);

    camera = Camera(glm::vec3(0.0f, 0.0f, 10.0f));

    graph.generateRandomGraph();
    adjustCameraToFitGraph();

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (gui.params.autoLayout) {
            graph.layoutStrength = gui.params.layoutStrength;
            graph.repulsionStrength = gui.params.repulsionStrength;
            graph.attractionStrength = gui.params.attractionStrength;
            graph.updateLayout(deltaTime * 10.0f);
        }
        if (gui.shouldRegenerate()) {
            graph.nodeCount = gui.params.nodeCount;
            graph.edgeProbability = gui.params.edgeProbability;
            graph.is3D = gui.params.is3D;

            switch (gui.params.graphType) {
            case 0:
                graph.generateRandomGraph();
                break;
            case 1:
                graph.generateGridGraph(gui.params.gridRows, gui.params.gridCols);
                break;
            case 2:
                graph.generateRingGraph();
                break;
            case 3:
                graph.generateStarGraph();
                break;
            }

            graph.normalizePositions();
            adjustCameraToFitGraph();
            gui.resetRegenerateFlag();
        }
        if (gui.shouldExportSVG()) {
            time_t now = time(0);
            struct tm tstruct;
            char filename[80];
            localtime_s(&tstruct, &now);
            strftime(filename, sizeof(filename), "graph_%Y%m%d_%H%M%S.svg", &tstruct);

            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            glm::mat4 projection = camera.getProjectionMatrix((float)width / (float)height, camera.zoom);
            glm::mat4 view = camera.getViewMatrix();

            graph.exportToSVG(filename, view, projection, width, height);
            gui.resetExportFlag();
        }

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glm::mat4 projection = camera.getProjectionMatrix((float)width / (float)height, camera.zoom);
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 MVP = projection * view;

        std::vector<std::pair<int, int>> edgePairs;
        for (const auto& edge : graph.edges) {
            edgePairs.emplace_back(edge.from, edge.to);
        }

        if (gui.params.showEdges && !graph.edges.empty()) {
            std::vector<glm::vec3> positions;
            for (const auto& node : graph.nodes) {
                positions.push_back(node.position);
            }
            renderer.renderEdges(positions, edgePairs, MVP);
        }

        if (gui.params.showNodes && !graph.nodes.empty()) {
            std::vector<glm::vec3> positions;
            for (const auto& node : graph.nodes) {
                positions.push_back(node.position);
            }
            renderer.renderNodes(positions, MVP);
        }

        gui.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    gui.shutdown();
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(1, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(2, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(3, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(4, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.processKeyboard(5, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.processKeyboard(6, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        camera.processMouseMovement(xoffset, yoffset);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.processMouseScroll(yoffset);
}
