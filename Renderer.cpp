#include "Renderer.h"
#include <iostream>

Renderer::Renderer() : VAO(0), VBO(0), EBO(0), shaderProgram(0) {
    edgeVerticesCache.reserve(10000);
}

Renderer::~Renderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
}

void Renderer::initialize() {
    shaderProgram = createShader(std::string(vertexShaderSource), std::string(fragmentShaderSource));

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Renderer::renderNodes(const std::vector<glm::vec3>& positions, const glm::mat4& MVP) {
    if (positions.empty()) return;

    glUseProgram(shaderProgram);

    int colorLoc = glGetUniformLocation(shaderProgram, "color");
    glUniform3f(colorLoc, 0.0f, 1.0f, 0.0f);

    int mvpLoc = glGetUniformLocation(shaderProgram, "MVP");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &MVP[0][0]);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_DYNAMIC_DRAW);

    glPointSize(8.0f);
    glDrawArrays(GL_POINTS, 0, positions.size());

    glBindVertexArray(0);
}

void Renderer::renderEdges(const std::vector<glm::vec3>& positions, const std::vector<std::pair<int, int>>& edges, const glm::mat4& MVP) {
    if (edges.empty() || positions.empty()) return;

    glUseProgram(shaderProgram);

    int colorLoc = glGetUniformLocation(shaderProgram, "color");
    glUniform3f(colorLoc, 0.7f, 0.7f, 0.7f);

    int mvpLoc = glGetUniformLocation(shaderProgram, "MVP");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &MVP[0][0]);

    glBindVertexArray(VAO);

    edgeVerticesCache.clear();
    edgeVerticesCache.reserve(edges.size() * 2);

    for (const auto& edge : edges) {
        if (edge.first < positions.size() && edge.second < positions.size()) {
            edgeVerticesCache.push_back(positions[edge.first]);
            edgeVerticesCache.push_back(positions[edge.second]);
        }
    }

    if (!edgeVerticesCache.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, edgeVerticesCache.size() * sizeof(glm::vec3),
            edgeVerticesCache.data(), GL_DYNAMIC_DRAW);

        glLineWidth(1.5f);
        glDrawArrays(GL_LINES, 0, edgeVerticesCache.size());
    }

    glBindVertexArray(0);
}

GLuint Renderer::createShader(const std::string& vertexCode, const std::string& fragmentCode) {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vShaderCode = vertexCode.c_str();
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "顶点着色器编译错误: " << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fShaderCode = fragmentCode.c_str();
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "片段着色器编译错误: " << infoLog << std::endl;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "着色器程序链接错误: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}
