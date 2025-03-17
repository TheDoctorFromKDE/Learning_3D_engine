#pragma once

void keyDown(unsigned char key, int x, int y);
void keyUp(unsigned char key, int x, int y);
void specialKeyDown(int key, int x, int y);
void specialKeyUp(int key, int x, int y);
void passiveMotion(int x, int y);
void moveCameraUp(float deltaTime);
void moveCameraDown(float deltaTime);
void processMouseMovementWrapper(int x, int y);