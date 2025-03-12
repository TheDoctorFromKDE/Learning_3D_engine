#pragma once

extern float cameraPosX;
extern float cameraPosY;
extern float cameraPosZ;
extern float cameraFrontX;
extern float cameraFrontY;
extern float cameraFrontZ;
extern float cameraUpX;
extern float cameraUpY;
extern float cameraUpZ;

void applyCameraTransformations();
void moveCameraForward(float distance);
void moveCameraBackward(float distance);
void moveCameraLeft(float distance);
void moveCameraRight(float distance);
void processMouseMovement(int x, int y);