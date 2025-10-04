#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 pos) :
    front(glm::vec3(0.0f, 0.0f, -1.0f)),
    worldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
    yaw(-90.0f),
    pitch(0.0f),
    movementSpeed(2.5f),
    mouseSensitivity(0.1f),
    zoom(45.0f),
    distance(10.0f),
    target(glm::vec3(0.0f)),
    firstMouse(true),
    lastX(400.0f),
    lastY(300.0f)
{
    position = pos;
    distance = glm::length(position - target);
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(position, target, up);
}

glm::mat4 Camera::getProjectionMatrix(float aspect, float fov, float near, float far) {
    return glm::perspective(glm::radians(fov), aspect, near, far);
}

void Camera::processKeyboard(int direction, float deltaTime) {
    float velocity = movementSpeed * deltaTime;

    if (direction == 1) {
        glm::vec3 forward = glm::normalize(target - position);
        position += forward * velocity;
        target += forward * velocity;
    }
    if (direction == 2) {
        glm::vec3 forward = glm::normalize(target - position);
        position -= forward * velocity;
        target -= forward * velocity;
    }
    if (direction == 3) {
        position -= right * velocity;
        target -= right * velocity;
    }
    if (direction == 4) {
        position += right * velocity;
        target += right * velocity;
    }
    if (direction == 5) {
        position += up * velocity;
        target += up * velocity;
    }
    if (direction == 6) {
        position -= up * velocity;
        target -= up * velocity;
    }
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (constrainPitch) {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    updateCameraVectors();

    position = target - front * distance;
}

void Camera::processMouseScroll(float yoffset) {
    distance -= yoffset * 0.5f;

    if (distance < 2.0f)
        distance = 2.0f;
    if (distance > 50.0f)
        distance = 50.0f;

    position = target - front * distance;
}

void Camera::setMousePosition(float xpos, float ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
}

void Camera::updateCameraVectors() {
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

void Camera::setTarget(glm::vec3 newTarget) {
    target = newTarget;
    position = target - front * distance;
}
