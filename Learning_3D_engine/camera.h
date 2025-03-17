#pragma once

#include <glm/glm.hpp>

extern float cameraPosX, cameraPosY, cameraPosZ;
extern float cameraFrontX, cameraFrontY, cameraFrontZ;
extern glm::vec3 cameraFront;
extern float cameraUpX, cameraUpY, cameraUpZ;
extern float speedCamera;
extern float cameraYaw, cameraPitch;

void moveCameraForward(float deltaTime);
void moveCameraBackward(float deltaTime);
void moveCameraLeft(float deltaTime);
void moveCameraRight(float deltaTime);
void moveCameraUp(float deltaTime);
void moveCameraDown(float deltaTime);
void processMouseMovement(float deltaX, float deltaY);