#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <cstdlib>
#include "camera.h"
#include "lighting.h"

int windowWidth = 800;
int windowHeight = 600;
int windowPosX = 100;
int windowPosY = 100;
bool isMouseCaptured = false;
DWORD lastTime = 0;
bool keys[256];
int centerX, centerY;

void drawColumn(float x, float z, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glColor3f(r, g, b); // Různé barvy
    glBegin(GL_QUADS);
    // Přední strana
    glVertex3f(-0.5f, 0.0f, 0.5f);
    glVertex3f(0.5f, 0.0f, 0.5f);
    glVertex3f(0.5f, 2.0f, 0.5f);
    glVertex3f(-0.5f, 2.0f, 0.5f);
    // Zadní strana
    glVertex3f(-0.5f, 0.0f, -0.5f);
    glVertex3f(0.5f, 0.0f, -0.5f);
    glVertex3f(0.5f, 2.0f, -0.5f);
    glVertex3f(-0.5f, 2.0f, -0.5f);
    // Levá strana
    glVertex3f(-0.5f, 0.0f, -0.5f);
    glVertex3f(-0.5f, 0.0f, 0.5f);
    glVertex3f(-0.5f, 2.0f, 0.5f);
    glVertex3f(-0.5f, 2.0f, -0.5f);
    // Pravá strana
    glVertex3f(0.5f, 0.0f, -0.5f);
    glVertex3f(0.5f, 0.0f, 0.5f);
    glVertex3f(0.5f, 2.0f, 0.5f);
    glVertex3f(0.5f, 2.0f, -0.5f);
    // Horní strana
    glVertex3f(-0.5f, 2.0f, -0.5f);
    glVertex3f(0.5f, 2.0f, -0.5f);
    glVertex3f(0.5f, 2.0f, 0.5f);
    glVertex3f(-0.5f, 2.0f, 0.5f);
    glEnd();
    glPopMatrix();
}

void display() {
    ULONGLONG currentTime = GetTickCount64();
    float deltaTime = (currentTime - lastTime) / 1000.0f; // Převod na sekundy
    lastTime = currentTime;

    if (keys['w']) {
        moveCameraForward(2.0f * deltaTime);
    }
    if (keys['s']) {
        moveCameraBackward(2.0f * deltaTime);
    }
    if (keys['a']) {
        moveCameraRight(2.0f * deltaTime);  // Prohozeno moveCameraLeft -> moveCameraRight
    }
    if (keys['d']) {
        moveCameraLeft(2.0f * deltaTime);   // Prohozeno moveCameraRight -> moveCameraLeft
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    applyCameraTransformations();
    setLightingPosition(); // Nastavení pozice světla po aplikaci transformačních operací kamery

    // Vykreslení sloupů
    for (int i = -5; i <= 5; ++i) {
        for (int j = -5; j <= 5; ++j) {
            float r = (i + 5) / 10.0f;
            float g = (j + 5) / 10.0f;
            float b = 0.5f;
            drawColumn(i * 5.0f, j * 5.0f, r, g, b);
        }
    }

    glutSwapBuffers();
}

void initialize() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, static_cast<double>(windowWidth) / windowHeight, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);

    glClearColor(0.5, 0.5, 0.5, 1.0); // Šedé pozadí
    glEnable(GL_DEPTH_TEST);
    initializeLighting();

    // Skrytí kurzoru a nastavení omezení pohybu kurzoru
    glutSetCursor(GLUT_CURSOR_NONE);
    RECT rect;
    GetClientRect(GetForegroundWindow(), &rect);
    MapWindowPoints(GetForegroundWindow(), nullptr, (POINT*)&rect, 2);
    ClipCursor(&rect);
    SetCapture(GetForegroundWindow());
    isMouseCaptured = true;

    // Nastavení pozice myši na střed okna
    centerX = windowWidth / 2;
    centerY = windowHeight / 2;
    glutWarpPointer(centerX, centerY);
}

void keyDown(unsigned char key, int x, int y) {
    keys[key] = true;

    if (key == 27) { // ESC key
        if (isMouseCaptured) {
            ClipCursor(NULL);
            ReleaseCapture();
            glutSetCursor(GLUT_CURSOR_INHERIT); // Zobrazení kurzoru
            isMouseCaptured = false;
        }
        else {
            RECT rect;
            GetClientRect(GetForegroundWindow(), &rect);
            MapWindowPoints(GetForegroundWindow(), nullptr, (POINT*)&rect, 2);
            ClipCursor(&rect);
            SetCapture(GetForegroundWindow());
            glutSetCursor(GLUT_CURSOR_NONE); // Skrytí kurzoru
            isMouseCaptured = true;

            // Nastavení pozice myši na střed okna
            glutWarpPointer(centerX, centerY);
        }
    }
}

void keyUp(unsigned char key, int x, int y) {
    keys[key] = false;
}

void passiveMotion(int x, int y) {
    if (isMouseCaptured && (x != centerX || y != centerY)) {
        int dx = x - centerX;
        int dy = y - centerY;

        processMouseMovement(dx, dy);

        // Nastavení pozice myši zpět na střed okna
        glutWarpPointer(centerX, centerY);
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(windowPosX, windowPosY);
    glutCreateWindow("3D Space with Colored Columns");

    initialize();

    glutDisplayFunc(display);
    glutIdleFunc(display); // Přidáno pro kontinuální aktualizaci
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutPassiveMotionFunc(passiveMotion); // Pro pohyb myší

    lastTime = GetTickCount64(); // Inicializace času
    memset(keys, 0, sizeof(keys)); // Inicializace pole kláves

    glutMainLoop();

    return 0;
}