#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include <algorithm>

struct Node {
    int id;
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 force;

    Node(int _id = 0, glm::vec3 _pos = glm::vec3(0.0f))
        : id(_id), position(_pos), velocity(0.0f), force(0.0f) {
    }
};

struct Edge {
    int from;
    int to;
    float weight;

    Edge(int _from = 0, int _to = 0, float _weight = 1.0f)
        : from(_from), to(_to), weight(_weight) {
    }
};

class Graph {
public:
    std::vector<Node> nodes;
    std::vector<Edge> edges;

    int nodeCount = 20;
    float edgeProbability = 0.3f;
    bool is3D = true;
    float layoutStrength = 0.1f;
    float repulsionStrength = 100.0f;
    float attractionStrength = 0.1f;

    Graph();

    void generateRandomGraph();
    void generateGridGraph(int rows, int cols);
    void generateRingGraph();
    void generateStarGraph();

    void updateLayout(float deltaTime);
    void applyForceDirectedLayout();
    void normalizePositions();

    void clear();
    void exportToSVG(const std::string& filename, const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix, int width, int height) const;

private:
    void generateEdges();
    void addEdge(int from, int to, float weight = 1.0f);
};
