#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

class Renderer {
public:
    GLuint VAO, VBO, EBO;
    GLuint shaderProgram;

    Renderer();
    ~Renderer();

    void initialize();
    void renderNodes(const std::vector<glm::vec3>& positions, const glm::mat4& MVP);
    void renderEdges(const std::vector<glm::vec3>& positions, const std::vector<std::pair<int, int>>& edges, const glm::mat4& MVP);

private:
    GLuint createShader(const std::string& vertexCode, const std::string& fragmentCode);
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        uniform mat4 MVP;
        void main() {
            gl_Position = MVP * vec4(aPos, 1.0);
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        uniform vec3 color;
        void main() {
            FragColor = vec4(color, 1.0);
        }
    )";
    std::vector<glm::vec3> edgeVerticesCache;
};
