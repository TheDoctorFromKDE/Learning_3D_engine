#define _USE_MATH_DEFINES
#include <windows.h>
#include "camera.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

// Globální proměnné pro směr kamery
float cameraPosX = 0.0f, cameraPosY = 10.0f, cameraPosZ = 20.0f;
float cameraFrontX = 0.0f, cameraFrontY = 0.0f, cameraFrontZ = -1.0f;
float cameraUpX = 0.0f, cameraUpY = 1.0f, cameraUpZ = 0.0f;
float yaw = -90.0f; // Inicializace na -90.0 stupňů, protože initialně směřuje na -Z
float pitch = 0.0f;

void applyCameraTransformations() {
    gluLookAt(cameraPosX, cameraPosY, cameraPosZ,
        cameraPosX + cameraFrontX, cameraPosY + cameraFrontY, cameraPosZ + cameraFrontZ,
        cameraUpX, cameraUpY, cameraUpZ);
}

void moveCameraForward(float distance) {
    cameraPosX += cameraFrontX * distance;
    cameraPosZ += cameraFrontZ * distance;
}

void moveCameraBackward(float distance) {
    cameraPosX -= cameraFrontX * distance;
    cameraPosZ -= cameraFrontZ * distance;
}

void moveCameraRight(float distance) {
    float cameraRightX = cameraFrontZ;
    float cameraRightZ = -cameraFrontX;
    cameraPosX -= cameraRightX * distance;
    cameraPosZ -= cameraRightZ * distance;
}

void moveCameraLeft(float distance) {
    float cameraRightX = cameraFrontZ;
    float cameraRightZ = -cameraFrontX;
    cameraPosX += cameraRightX * distance;
    cameraPosZ += cameraRightZ * distance;
}

// Add Up & Down camera movement

void processMouseMovement(int xOffset, int yOffset) {
    float sensitivity = 0.1f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch -= yOffset;  // Invertovat yOffset pro správné otáčení nahoru a dolů

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    float frontX = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    float frontY = sin(glm::radians(pitch));
    float frontZ = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    // Normalizace
    float length = sqrt(frontX * frontX + frontY * frontY + frontZ * frontZ);
    cameraFrontX = frontX / length;
    cameraFrontY = frontY / length;
    cameraFrontZ = frontZ / length;
}