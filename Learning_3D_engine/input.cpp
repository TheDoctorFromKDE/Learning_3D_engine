#include <windows.h>
#include <GL/freeglut.h>
#include <iostream>
#include <glm/glm.hpp>
#include "camera.h"
#include "input.h"
#include "colors.h"
#include "globals.h" // Zahrnutí globals.h

extern bool keys[256];

void keyDown(unsigned char key, int x, int y) {
    keys[key] = true;

    if (key == 27) { // ESC key
        if (isMouseCaptured) {
            ClipCursor(NULL);
            ReleaseCapture();
            glutSetCursor(GLUT_CURSOR_INHERIT);
            isMouseCaptured = false;
        }
        else {
            RECT rect;
            GetClientRect(GetForegroundWindow(), &rect);
            MapWindowPoints(GetForegroundWindow(), nullptr, (POINT*)&rect, 2);
            ClipCursor(&rect);
            SetCapture(GetForegroundWindow());
            glutSetCursor(GLUT_CURSOR_NONE);
            isMouseCaptured = true;

            glutWarpPointer(centerX, centerY);
        }
    }

    if (key == 'r') {
        initializeColumnColors();
        std::cout << "Colors randomized" << std::endl;
    }
}

void keyUp(unsigned char key, int x, int y) {
    keys[key] = false;
}

void specialKeyDown(int key, int x, int y) {
    keys[key] = true;
}

void specialKeyUp(int key, int x, int y) {
    keys[key] = false;
}

void passiveMotion(int x, int y) {
    if (isMouseCaptured && (x != centerX || y != centerY)) {
        int deltaX = x - centerX;
        int deltaY = centerY - y; // Inverze Y osy

        processMouseMovement(static_cast<float>(deltaX), static_cast<float>(deltaY)); // Volání správné funkce

        glutWarpPointer(centerX, centerY); // Resetování kurzoru do středu okna
    }
}

// Obalová funkce pro přetypování int na float
void processMouseMovementWrapper(int x, int y) {
    processMouseMovement(static_cast<float>(x), static_cast<float>(y));
}