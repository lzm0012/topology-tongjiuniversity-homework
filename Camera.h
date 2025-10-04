#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float yaw;
    float pitch;

    float movementSpeed;
    float mouseSensitivity;
    float zoom;
    float distance;

    bool firstMouse;
    float lastX, lastY;

    glm::vec3 target;

    Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 10.0f));

    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix(float aspect, float fov = 45.0f,
        float near = 0.1f, float far = 100.0f);

    void processKeyboard(int direction, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void processMouseScroll(float yoffset);
    void setMousePosition(float xpos, float ypos);
    void updateCameraVectors();

    void setTarget(glm::vec3 newTarget);
};
