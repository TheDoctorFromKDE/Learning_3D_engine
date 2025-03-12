#include <windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include "lighting.h"
#include "shader_program.h"

int windowWidth = 800;
int windowHeight = 600;
int windowPosX = 100;
int windowPosY = 100;
bool isMouseCaptured = false;
DWORD lastTime = 0;
bool keys[256];
int centerX, centerY;

GLuint VAO, VBO, NBO;
Shader* shaderProgram;

// Globální proměnné pro barvy sloupů
struct Color {
    float r, g, b;
};

std::vector<Color> columnColors;

void initializeColumnColors() {
    columnColors.clear();
    for (int i = 0; i < 11; ++i) {
        for (int j = 0; j < 11; ++j) {
            Color color = {
                static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
                static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
                static_cast<float>(rand()) / static_cast<float>(RAND_MAX)
            };
            columnColors.push_back(color);
        }
    }
}

void drawColumn(float x, float z, const Color& color) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    shaderProgram->setVec3("objectColor", glm::vec3(color.r, color.g, color.b));
    glDrawArrays(GL_QUADS, 0, 24);
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

    glm::mat4 view = glm::lookAt(glm::vec3(cameraPosX, cameraPosY, cameraPosZ),
    glm::vec3(cameraPosX + cameraFrontX, cameraPosY + cameraFrontY, cameraPosZ + cameraFrontZ),
    glm::vec3(cameraUpX, cameraUpY, cameraUpZ));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
    shaderProgram->use();
    shaderProgram->setMat4("view", view);
    shaderProgram->setMat4("projection", projection);
    shaderProgram->setVec3("lightPos", glm::vec3(1.0f, 1.0f, 1.0f));
    shaderProgram->setVec3("viewPos", glm::vec3(cameraPosX, cameraPosY, cameraPosZ));
    shaderProgram->setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

    // Aktivace VAO
    glBindVertexArray(VAO);

    // Vykreslení sloupů
    int index = 0;
    for (int i = -5; i <= 5; ++i) {
        for (int j = -5; j <= 5; ++j) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(i * 5.0f, 0.0f, j * 5.0f));
            shaderProgram->setMat4("model", model);
            drawColumn(i * 5.0f, j * 5.0f, columnColors[index++]);
        }
    }

    glBindVertexArray(0); // Deaktivace VAO

    glutSwapBuffers();
}

void initialize() {
    glewExperimental = GL_TRUE; // Potřebné pro některé implementace OpenGL
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error initializing GLEW: " << glewGetErrorString(err) << std::endl;
        exit(1); // Ukončení programu při chybě
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, static_cast<double>(windowWidth) / windowHeight, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);

    glClearColor(0.5, 0.5, 0.5, 1.0); // Šedé pozadí
    glEnable(GL_DEPTH_TEST);
    initializeLighting();

    // Inicializace shader programu
    shaderProgram = new Shader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");

    // Inicializace náhodných barev sloupů
    srand(static_cast<unsigned int>(time(0)));
    initializeColumnColors();

    // Inicializace VAO a VBO
    GLfloat vertices[] = {
        // Přední strana
        -0.5f, 0.0f,  0.5f,
         0.5f, 0.0f,  0.5f,
         0.5f, 2.0f,  0.5f,
        -0.5f, 2.0f,  0.5f,
        // Zadní strana
        -0.5f, 0.0f, -0.5f,
         0.5f, 0.0f, -0.5f,
         0.5f, 2.0f, -0.5f,
        -0.5f, 2.0f, -0.5f,
        // Levá strana
        -0.5f, 0.0f, -0.5f,
        -0.5f, 0.0f,  0.5f,
        -0.5f, 2.0f,  0.5f,
        -0.5f, 2.0f, -0.5f,
        // Pravá strana
         0.5f, 0.0f, -0.5f,
         0.5f, 0.0f,  0.5f,
         0.5f, 2.0f,  0.5f,
         0.5f, 2.0f, -0.5f,
         // Horní strana
         -0.5f, 2.0f, -0.5f,
          0.5f, 2.0f, -0.5f,
          0.5f, 2.0f,  0.5f,
         -0.5f, 2.0f,  0.5f
    };

    GLfloat normals[] = {
        // Přední strana
        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,
        // Zadní strana
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        // Levá strana
       -1.0f,  0.0f,  0.0f,
       -1.0f,  0.0f,  0.0f,
       -1.0f,  0.0f,  0.0f,
       -1.0f,  0.0f,  0.0f,
       // Pravá strana
       1.0f,  0.0f,  0.0f,
       1.0f,  0.0f,  0.0f,
       1.0f,  0.0f,  0.0f,
       1.0f,  0.0f,  0.0f,
       // Horní strana
       0.0f,  1.0f,  0.0f,
       0.0f,  1.0f,  0.0f,
       0.0f,  1.0f,  0.0f,
       0.0f,  1.0f,  0.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &NBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

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

    // Klávesová zkratka pro náhodné změny barev
    if (key == 'r') {
        initializeColumnColors();
        std::cout << "Colors randomized" << std::endl;
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