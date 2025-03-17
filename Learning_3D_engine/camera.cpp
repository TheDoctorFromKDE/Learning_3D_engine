#include "camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

float cameraPosX = 0.0f, cameraPosY = 10.0f, cameraPosZ = 20.0f;
float cameraFrontX = 0.0f, cameraFrontY = 0.0f, cameraFrontZ = -1.0f;
glm::vec3 cameraFront = glm::vec3(cameraFrontX, cameraFrontY, cameraFrontZ);
float cameraUpX = 0.0f, cameraUpY = 1.0f, cameraUpZ = 0.0f;
float cameraYaw = -90.0f, cameraPitch = 0.0f;
float speedCamera = 7.0f;

void moveCameraForward(float deltaTime) {
    float distance = speedCamera * deltaTime;
    cameraPosX += cameraFrontX * distance;
    cameraPosZ += cameraFrontZ * distance;
}

void moveCameraBackward(float deltaTime) {
    float distance = speedCamera * deltaTime;
    cameraPosX -= cameraFrontX * distance;
    cameraPosZ -= cameraFrontZ * distance;
}

void moveCameraLeft(float deltaTime) {
    float distance = speedCamera * deltaTime;
    glm::vec3 right = glm::normalize(glm::cross(glm::vec3(cameraFrontX, cameraFrontY, cameraFrontZ), glm::vec3(cameraUpX, cameraUpY, cameraUpZ)));
    cameraPosX -= right.x * distance;
    cameraPosZ -= right.z * distance;
}

void moveCameraRight(float deltaTime) {
    float distance = speedCamera * deltaTime;
    glm::vec3 right = glm::normalize(glm::cross(glm::vec3(cameraFrontX, cameraFrontY, cameraFrontZ), glm::vec3(cameraUpX, cameraUpY, cameraUpZ)));
    cameraPosX += right.x * distance;
    cameraPosZ += right.z * distance;
}

void moveCameraUp(float deltaTime) {
    cameraPosY += speedCamera * deltaTime;
}

void moveCameraDown(float deltaTime) {
    cameraPosY -= speedCamera * deltaTime;
}

void processMouseMovement(float deltaX, float deltaY) {
    const float sensitivity = 0.1f;
    deltaX *= sensitivity;
    deltaY *= sensitivity;

    cameraYaw += deltaX;
    cameraPitch += deltaY;

    if (cameraPitch > 89.0f) cameraPitch = 89.0f;
    if (cameraPitch < -89.0f) cameraPitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
    front.y = sin(glm::radians(cameraPitch));
    front.z = sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
    cameraFront = glm::normalize(front);

    cameraFrontX = cameraFront.x;
    cameraFrontY = cameraFront.y;
    cameraFrontZ = cameraFront.z;
}