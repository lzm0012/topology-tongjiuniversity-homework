#include "Graph.h"
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Graph::Graph() {
    nodes.reserve(500);
    edges.reserve(2500);
}

void Graph::clear() {
    nodes.clear();
    edges.clear();
}

void Graph::generateRandomGraph() {
    clear();

    nodes.reserve(nodeCount);
    edges.reserve(nodeCount * nodeCount / 4);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> pos_dist(-1.0f, 1.0f);

    for (int i = 0; i < nodeCount; ++i) {
        glm::vec3 pos;
        if (is3D) {
            pos = glm::vec3(
                pos_dist(gen) * 5.0f,
                pos_dist(gen) * 5.0f,
                pos_dist(gen) * 5.0f
            );
        }
        else {
            pos = glm::vec3(
                pos_dist(gen) * 5.0f,
                pos_dist(gen) * 5.0f,
                0.0f
            );
        }
        nodes.emplace_back(i, pos);
    }

    generateEdges();
}

void Graph::generateGridGraph(int rows, int cols) {
    clear();

    nodeCount = rows * cols;
    nodes.reserve(nodeCount);
    edges.reserve(rows * cols * 2);

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            glm::vec3 pos;
            if (is3D) {
                pos = glm::vec3(c * 1.5f - (cols - 1) * 0.75f,
                    r * 1.5f - (rows - 1) * 0.75f,
                    (r + c) * 0.5f - (rows + cols) * 0.25f);
            }
            else {
                pos = glm::vec3(c * 1.5f - (cols - 1) * 0.75f,
                    r * 1.5f - (rows - 1) * 0.75f,
                    0.0f);
            }
            nodes.emplace_back(r * cols + c, pos);
        }
    }
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            int current = r * cols + c;

            if (c < cols - 1) {
                addEdge(current, current + 1);
            }

            if (r < rows - 1) {
                addEdge(current, current + cols);
            }

            if (is3D && r < rows - 1 && c < cols - 1) {
                addEdge(current, current + cols + 1);
            }
        }
    }
}

void Graph::generateRingGraph() {
    clear();

    nodes.reserve(nodeCount);
    edges.reserve(nodeCount);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> noise_dist(0.0f, 0.1f);

    for (int i = 0; i < nodeCount; ++i) {
        float angle = 2.0f * M_PI * i / nodeCount;
        glm::vec3 pos;

        if (is3D) {
            float z = sin(angle * 2.0f) * 2.0f;
            pos = glm::vec3(cos(angle) * 5.0f, sin(angle) * 5.0f, z);
        }
        else {
            pos = glm::vec3(cos(angle) * 5.0f, sin(angle) * 5.0f, 0.0f);
        }

        pos.x += noise_dist(gen);
        pos.y += noise_dist(gen);
        if (is3D) pos.z += noise_dist(gen);

        nodes.emplace_back(i, pos);
    }

    for (int i = 0; i < nodeCount; ++i) {
        addEdge(i, (i + 1) % nodeCount);
    }
}

void Graph::generateStarGraph() {
    clear();

    nodes.reserve(nodeCount);
    edges.reserve(nodeCount - 1);

    nodes.emplace_back(0, glm::vec3(0.0f));

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> angle_dist(0.0f, 2.0f * M_PI);

    for (int i = 1; i < nodeCount; ++i) {
        float angle;
        if (is3D) {
            float z = std::uniform_real_distribution<float>(-1.0f, 1.0f)(gen);
            float r = sqrt(1.0f - z * z);
            angle = angle_dist(gen);
            float x = r * cos(angle);
            float y = r * sin(angle);
            nodes.emplace_back(i, glm::vec3(x, y, z) * 5.0f);
        }
        else {
            angle = angle_dist(gen);
            nodes.emplace_back(i, glm::vec3(cos(angle) * 5.0f, sin(angle) * 5.0f, 0.0f));
        }

        addEdge(0, i);
    }
}

void Graph::generateEdges() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> prob_dist(0.0f, 1.0f);

    for (int i = 0; i < nodeCount; ++i) {
        for (int j = i + 1; j < nodeCount; ++j) {
            if (prob_dist(gen) < edgeProbability) {
                addEdge(i, j);
            }
        }
    }
}

void Graph::addEdge(int from, int to, float weight) {
    if (from < 0 || from >= (int)nodes.size() ||
        to < 0 || to >= (int)nodes.size() ||
        from == to) {
        return;
    }

    for (const auto& edge : edges) {
        if ((edge.from == from && edge.to == to) || (edge.from == to && edge.to == from)) {
            return;
        }
    }

    edges.emplace_back(from, to, weight);
}

void Graph::updateLayout(float deltaTime) {
    applyForceDirectedLayout();

    for (auto& node : nodes) {
        node.velocity += node.force * deltaTime;
        node.velocity *= 0.9f;
        node.position += node.velocity * deltaTime;
        node.force = glm::vec3(0.0f);
    }
}

void Graph::applyForceDirectedLayout() {
    for (auto& node : nodes) {
        node.force = glm::vec3(0.0f);
    }

    const float maxRepulsionDistance = 15.0f;

    for (size_t i = 0; i < nodes.size(); ++i) {
        for (size_t j = i + 1; j < nodes.size(); ++j) {
            glm::vec3 diff = nodes[i].position - nodes[j].position;
            float distance = glm::length(diff);

            if (distance > 0.001f && distance < maxRepulsionDistance) {
                float force = repulsionStrength / (distance * distance);
                glm::vec3 direction = glm::normalize(diff);
                nodes[i].force += direction * force;
                nodes[j].force -= direction * force;
            }
        }
    }

    for (const auto& edge : edges) {
        glm::vec3 diff = nodes[edge.to].position - nodes[edge.from].position;
        float distance = glm::length(diff);

        if (distance > 0.001f) {
            float force = attractionStrength * distance * distance;
            glm::vec3 direction = glm::normalize(diff);
            nodes[edge.from].force += direction * force;
            nodes[edge.to].force -= direction * force;
        }
    }
}

void Graph::normalizePositions() {
    if (nodes.empty()) return;

    glm::vec3 min_pos = nodes[0].position;
    glm::vec3 max_pos = nodes[0].position;

    for (const auto& node : nodes) {
        min_pos = glm::min(min_pos, node.position);
        max_pos = glm::max(max_pos, node.position);
    }

    glm::vec3 center = (min_pos + max_pos) * 0.5f;
    glm::vec3 size = max_pos - min_pos;
    float max_size = glm::max(glm::max(size.x, size.y), size.z);

    if (max_size > 0.001f) {
        float scale = 5.0f / max_size;

        for (auto& node : nodes) {
            node.position = (node.position - center) * scale;
        }
    }
}

void Graph::exportToSVG(const std::string& filename, const glm::mat4& viewMatrix,
    const glm::mat4& projectionMatrix, int width, int height) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Unable to create SVG file: " << filename << std::endl;
        return;
    }
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    file << "<svg xmlns=\"http://www.w3.org/2000/svg\" ";
    file << "width=\"" << width << "\" height=\"" << height << "\" ";
    file << "viewBox=\"0 0 " << width << " " << height << "\">\n";

    file << "  <rect width=\"" << width << "\" height=\"" << height << "\" fill=\"#1a1a1a\"/>\n";

    glm::mat4 MVP = projectionMatrix * viewMatrix;

    file << "  <g id=\"edges\">\n";
    for (const auto& edge : edges) {
        if (edge.from < (int)nodes.size() && edge.to < (int)nodes.size()) {
            glm::vec4 clipFrom = MVP * glm::vec4(nodes[edge.from].position, 1.0f);
            if (clipFrom.w != 0.0f) clipFrom /= clipFrom.w;
            float fromX = (clipFrom.x * 0.5f + 0.5f) * width;
            float fromY = (1.0f - (clipFrom.y * 0.5f + 0.5f)) * height;
            glm::vec4 clipTo = MVP * glm::vec4(nodes[edge.to].position, 1.0f);
            if (clipTo.w != 0.0f) clipTo /= clipTo.w;
            float toX = (clipTo.x * 0.5f + 0.5f) * width;
            float toY = (1.0f - (clipTo.y * 0.5f + 0.5f)) * height;

            file << "    <line x1=\"" << fromX << "\" y1=\"" << fromY
                << "\" x2=\"" << toX << "\" y2=\"" << toY
                << "\" stroke=\"#b3b3b3\" stroke-width=\"1.5\"/>\n";
        }
    }
    file << "  </g>\n";
    file << "  <g id=\"nodes\">\n";
    for (const auto& node : nodes) {
        glm::vec4 clip = MVP * glm::vec4(node.position, 1.0f);
        if (clip.w != 0.0f) clip /= clip.w;
        float x = (clip.x * 0.5f + 0.5f) * width;
        float y = (1.0f - (clip.y * 0.5f + 0.5f)) * height;

        file << "    <circle cx=\"" << x << "\" cy=\"" << y
            << "\" r=\"4\" fill=\"#00ff00\"/>\n";
    }
    file << "  </g>\n";

    file << "</svg>\n";
    file.close();

    std::cout << "SVG exported successfully: " << filename << std::endl;
}
